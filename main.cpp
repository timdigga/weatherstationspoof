#include <string>
#include <cstring>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>

using namespace std;

const int SAMPLE_RATE = 2000000;
const int SYMBOL_RATE = 2000;

uint8_t nibbles[9];
vector<int> data;
vector<uint8_t> out_cu8;
vector<int8_t> out_cs8;

// ANSI escape codes for color
#define RESET_COLOR   "\033[0m"
#define BOLD_TEXT     "\033[1m"
#define BLUE_TEXT     "\033[34m"
#define CYAN_TEXT     "\033[36m"
#define GREEN_TEXT    "\033[32m"

void usage(const char *cmd)
{
    fprintf(stderr, "Usage: %s [-o file] [-i ID] [-c channel] [-t temp] [-h humidity]\n", cmd);
    exit(1);
}

void add_sync(vector<int> &v)
{
    v.push_back(1);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
}

void add_zero(vector<int> &v)
{
    v.push_back(1);
    v.push_back(0);
    v.push_back(0);
}

void add_one(vector<int> &v)
{
    v.push_back(1);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
}

void generate_data()
{
    for (int k = 0 ; k < 12 ; k++) {
        add_sync(data);
        for (int i = 0 ; i < 9 ; i++) {
            uint8_t mask = 0x08;
            for (int j = 0 ; j < 4 ; j++) {
                if (nibbles[i] & mask) {
                    add_one(data);
                } else {
                    add_zero(data);
                }
                mask >>= 1;
            }
        }
    }
}

void generate_samples()
{
    int spb = SAMPLE_RATE / SYMBOL_RATE; // samples per bit
    for (int i = 0 ; i < (int) data.size() ; i++) {
        for (int j = 0 ; j < spb ; j++) {
            out_cu8.push_back(data[i] ? 255 : 127);
            out_cu8.push_back(127);
            out_cs8.push_back(data[i] ? 127 : 0);
            out_cs8.push_back(0);
        }
    }
}

void save_sub(const string &fname, int freq) {
    printf("Saving to %s\n", fname.c_str());
    FILE *f = fopen(fname.c_str(), "w");
    fprintf(f, "Filetype: Flipper SubGhz RAW File\n"
               "Version: 1\n"
               "Frequency: %d\n"
               "Preset: FuriHalSubGhzPresetOok650Async\n"
               "Protocol: RAW", freq);
    int one_len = 500;
    int zero_len = 500;

    int prev_bit = -1;
    int prev_len = 0;
    vector<int> raw_data;
    for (int i = 0; i < (int)data.size(); i++) {
        if (prev_bit >= 0 && prev_bit != data[i]) {
            raw_data.push_back(prev_len);
            prev_len = 0;
        }
        if (data[i]) {
            prev_len += one_len;
        } else {
            prev_len -= zero_len;
        }
        prev_bit = data[i];
    }
    raw_data.push_back(prev_len);
    if (prev_bit) {
        raw_data.push_back(-zero_len);
    }
    for (int i = 0; i < (int)raw_data.size(); i++) {
        if (i % 512 == 0) {
            fprintf(f, "\nRAW_Data: ");
        }
        fprintf(f, "%d ", raw_data[i]);
    }
    fclose(f);
}

template<typename T>
void save_to_file(const string &fname, vector<T> &out)
{
    printf("Saving to %s\n", fname.c_str());
    FILE *f = fopen(fname.c_str(), "wb");
    fwrite(out.data(), 1, out.size(), f);
    printf("Saved!");
    fclose(f);
}

int main(int argc, char *argv[])
{
    int opt;
    float temp_f = 26.3;
    uint8_t id = 244;
    int8_t humidity = 20, channel = 1;
    int freq = 433920000;  // Set frequency to 433.92 MHz
    string fname;

    // Modernized and styled header
    printf("\n");
    printf(BLUE_TEXT "---------------------------\n" RESET_COLOR);
    printf(GREEN_TEXT "- " BOLD_TEXT "Weather Station Spoofer" RESET_COLOR " -\n");
    printf(BLUE_TEXT "---------------------------\n\n" RESET_COLOR);

    while ((opt = getopt(argc, argv, "i:c:t:h:o:f:")) != -1) {
        switch (opt) {
            case 'i':
                id = atoi(optarg);
                break;
            case 'c':
                channel = atoi(optarg);
                if (channel < 1 || channel > 3) {
                    fprintf(stderr, "Invalid argument: channel must be in [1, 3]\n");
                    exit(1);
                }
                break;
            case 't':
                temp_f = atof(optarg);
                if (temp_f < -204.7 || temp_f > 204.7) {
                    fprintf(stderr, "Invalid argument: temperature must be in [-204.7, 204.7]\n");
                    exit(1);
                }
                break;
            case 'h':
                humidity = atoi(optarg);
                if (humidity < 0 || humidity > 100) {
                    fprintf(stderr, "Invalid argument: humidity must be in [0, 100]\n");
                    exit(1);
                }
                break;
            case 'o':
                fname = optarg;
                break;
            case 'f':
                freq = atoi(optarg);
                break;
            default:
                usage(argv[0]);
                break;
        }
    }
    
    printf("ID: %d, channel: %d, temperature: %.2f, humidity: %d\n", id, channel, temp_f, humidity);

    int16_t temp = (int16_t)(temp_f * 10);

    nibbles[0] = (id >> 4) & 0x0f;
    nibbles[1] = id & 0x0f;
    nibbles[2] = 7 + channel;
    nibbles[3] = (temp >> 8) & 0x0f;
    nibbles[4] = (temp >> 4) & 0x0f;
    nibbles[5] = temp & 0x0f;
    nibbles[6] = 0x0f;
    nibbles[7] = (humidity >> 4) & 0x0f;
    nibbles[8] = humidity & 0x0f;

    generate_data();
    generate_samples();

    if (!fname.empty()) {
        save_sub(fname + ".sub", freq);
        printf("Saved .sub files!");
        return 0;
    }

    fprintf(stderr, "No output file specified. Use the -o option to specify a file name.\n");
    return 1;
}
