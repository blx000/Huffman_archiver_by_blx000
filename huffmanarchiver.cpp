#include <bits/stdc++.h>

using namespace std;

typedef long long ll;

FILE *f, *g;
//пишем структуры XD

struct Forest {
    int sym;
    int weight;
    int root;
};

struct Tree {
    int left = -1, right = -1, parent = -1;
};

Forest forest[256];
Tree tree[520];


//реализация функции поиска минимума
pair <int, int> getMin(int symcnt) {
    int min1 = 0, min2 = 1;
    if (forest[min1].weight > forest[min2].weight) {
        swap(min1, min2);
    }

    for (int i = 2; i < symcnt; i++) {
        if (forest[i].weight < forest[min1].weight) {
            min2 = min1;
            min1 = i;
        } else if (forest[i].weight < forest[min2].weight) {
            min2 = i;
        }
    }

    return {min1, min2};
}

void decode(char* in, char* out)
{
    f = fopen(in, "rb");
    g = fopen(out, "wb");

    unsigned char ch1;
    unsigned char prev;
    int prev_cnt = 0;

    int tp = 0;
    int need_bits = 8;

    int cnt_char;

    int index_char = 0;
    char ch;
    int s = 0;
    int len_s = 0;
    map <int, map<int, char>> mp;

    int shift = 30;

    ll ln = 0;
    fscanf(f, "%c", &ch1);
    ln |= (ch1 << 56);
    fscanf(f, "%c", &ch1);
    ln |= (ch1 << 48);
    fscanf(f, "%c", &ch1);
    ln |= (ch1 << 40);
    fscanf(f, "%c", &ch1);
    ln |= (ch1 << 32);
    fscanf(f, "%c", &ch1);
    ln |= (ch1 << 24);
    fscanf(f, "%c", &ch1);
    ln |= (ch1 << 16);
    fscanf(f, "%c", &ch1);
    ln |= (ch1 << 8);
    fscanf(f, "%c", &ch1);
    ln |= ch1;

    fscanf(f, "%c", &ch1);

    cnt_char = int(ch1);
    if (cnt_char == 0) {
        cnt_char = 256;
    }

    while (fscanf(f, "%c", &ch1) != -1) {
        unsigned char ans = ch1;

        if (tp == 2) {
            for (int i = 7; i > max(7 - need_bits, -1); i--) {
                s |= ((((int)ans >> i) & 1) << shift);
                shift--;
            }
        }
        need_bits -= min(8, need_bits);

        if (need_bits == 0) {
            if (tp == 0) {
                ch = ans;
                need_bits = 8;
            } else if (tp == 1) {
                need_bits = int(ans) / 8 * 8;
                if (int(ans) % 8) {
                    need_bits += 8;
                }
                len_s = int(ans);
            } else {
                mp[len_s][s] = ch;
                s = 0;
                need_bits = 8;
                shift = 30;
            }
            tp += 1;
            tp %= 3;
            if (tp == 0) {
                index_char++;
                if (index_char >= cnt_char) {
                    break;
                }
            }
        }
    }

    vector <unsigned char> vc;
    while (fscanf(f, "%c", &ch1) != -1) {
        vc.push_back(ch1);
    }
    s = 0;
    shift = 30;
    ll find_symbols = 0;
    for (int ind1 = 0; ind1 < vc.size(); ind1++) {
        for (int ind2 = 7; ind2 >= 0; ind2--) {
            s |= ((((int)vc[ind1] >> ind2) & 1) << shift);
            shift--;
            if (mp[30 - shift].find(s) != mp[30 - shift].end()) {
                fprintf(g, "%c", char(mp[30 - shift][s]));
                s = 0;
                shift = 30;
                find_symbols++;
                if (find_symbols >= ln) {
                    break;
                }
            }
        }
        if (find_symbols >= ln) {
            break;
        }
    }

    fclose(f);
    fclose(g);
}

void encode(char* in, char* out)
{
    int freq[256];
    f = fopen(in, "rb");
    g = fopen(out, "wb");
    //зануляем частотный словарь
    for (int i = 0; i < 256; i++) {
        freq[i] = 0;
    }
    unsigned char ch;
    ll ln = 0;
    //заполняем частотный словарь
    while (fscanf(f, "%c", &ch) != -1) {
        freq[ch]++;
        ln++;
    }
    //инициализация фореста
    int symind = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] != 0) {
            forest[symind].sym = i;
            forest[symind].weight = freq[i];
            forest[symind].root = symind;
            symind += 1;
        }
    }

    int trind = symind;
    int forsize = symind;

    //инициализация три
    while (forsize > 1) {
        pair <int, int> minP = getMin(forsize);
        int lt = minP.first, rt = minP.second;

        tree[trind].left = forest[lt].root;
        tree[trind].right = forest[rt].root;
        tree[trind].parent = -1;

        tree[forest[lt].root].parent = trind;
        tree[forest[rt].root].parent = trind;

        forest[lt].weight = forest[lt].weight + forest[rt].weight;
        forest[lt].root = trind;
        forest[rt] = forest[forsize - 1];
        forsize--;
        trind++;
    }
    map <unsigned char, string> codes;
    symind = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            string code;
            int current = symind;

            while (tree[current].parent != -1) {
                int last = current;
                current = tree[current].parent;
                if (tree[current].left == last) {
                    code += "0";
                } else {
                    code += "1";
                }
            }
            reverse(code.begin(), code.end());
            codes[i] = code;
            symind++;
        }
    }
    fclose(f);
    f = fopen(in, "rb");
    unsigned char ch1;
    int byte = 0, cnt_bit = 7;
    int cntt = 0;

    char mask = 0b11111111;
    fprintf(g, "%c", char(mask & (ln >> 56)));
    fprintf(g, "%c", char(mask & (ln >> 48)));
    fprintf(g, "%c", char(mask & (ln >> 40)));
    fprintf(g, "%c", char(mask & (ln >> 32)));
    fprintf(g, "%c", char(mask & (ln >> 24)));
    fprintf(g, "%c", char(mask & (ln >> 16)));
    fprintf(g, "%c", char(mask & (ln >> 8)));
    fprintf(g, "%c", char(mask & ln));

    fprintf(g, "%c", char(codes.size()));
    if (codes.size() == 1) {
        codes.begin()->second = "0";
    }
    for (auto [ch, kod] : codes) {
        fprintf(g, "%c", char(ch));
        fprintf(g, "%c", char(kod.length()));
        for (int i = 0; i < kod.length(); i++) {
            byte += ((kod[i] - '0') << cnt_bit);
            cnt_bit -= 1;
            if (cnt_bit == -1){
                fprintf(g, "%c", char(byte));
                byte = 0;
                cnt_bit = 7;
            }
        }
        if (cnt_bit < 7) {
            fprintf(g, "%c", char(byte));
            byte = 0;
            cnt_bit = 7;
        }
    }
    while (fscanf(f, "%c", &ch1) != -1) {
        string tempcode = codes[ch1];
        for (int i = 0; i < tempcode.length(); i++) {
            byte += ((tempcode[i] - '0') << cnt_bit);
            cnt_bit -= 1;
            if (cnt_bit == -1) {
                fprintf(g, "%c", char(byte));
                byte = 0;
                cnt_bit = 7;
            }
        }
    }
    if (cnt_bit < 7) {
        fprintf(g, "%c", char(byte));
        byte = 0;
        cnt_bit = 7;
    }
    fclose(f);
    fclose(g);
}

int32_t main(int argc, char *argv[]) {
    if (!strcmp("encode", argv[1])) {
        encode(argv[3], argv[2]);
    } else {
        decode(argv[2], argv[3]);
    }
}



