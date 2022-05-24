#include <iostream>
#include <string>
#include <map>
#include <cmath>
#include <pthread.h>
#include <unistd.h>
#include <bitset>
#include <bits/stdc++.h>
#include <vector>

struct arguments
{
    char c;
    int frequency = 0;
    int dec_number;
    int bit_width;
    std::vector<std::string> *ptr;
    std::string binaryPresentation;
};

struct message
{
    char c;
    std::map<std::string, char> *dictPtr;
    std::string message;
};

void *freqCounter(void *void_ptr)
{
    struct arguments *arg_ptr = (struct arguments *)void_ptr;
    std::vector<std::string> &pointer = *arg_ptr->ptr;
    for (const auto &i : pointer)
    {
        if (arg_ptr->binaryPresentation == i)
            arg_ptr->frequency += 1;
    }
    return nullptr;
}

void *decToBinary(void *void_ptr)
{
    struct arguments *arg_ptr = (struct arguments *)void_ptr;
    int base_value = arg_ptr->dec_number;
    int bit_width = arg_ptr->bit_width;
    std::string binary = "";
    for (int i = 0; i < bit_width; i++)
    {
        if (base_value % 2 == 0)
        {
            binary += "0";
        }
        else
        {
            binary += "1";
        }

        base_value /= 2;
    }

    reverse(binary.begin(), binary.end());
    arg_ptr->binaryPresentation = binary;
    freqCounter(void_ptr);

    return nullptr;
}

void *decompressMessage(void *void_ptr)
{
    struct message *arg_ptr = (struct message *)void_ptr;
    std::map<std::string, char> &dictPtr = *arg_ptr->dictPtr;
    arg_ptr->c = dictPtr[arg_ptr->message];
    return nullptr;
}

// Function to return bit-width of highest value
int ceil_log(int value)
{
    int answer = ceil(log2(value));
    return answer;
}

int main()
{
    std::string input;
    getline(std::cin, input);

    int n_thread = stoi(input);

    char character;
    int val;
    int max = 0;

    std::string line;
    pthread_t *tid = new pthread_t[n_thread];
    struct arguments *arg = new arguments[n_thread];

    std::vector<int> value;
    std::vector<char> word;

    // Find bit-width
    for (int i = 0; i < n_thread; i++)
    {
        getline(std::cin, line);
        character = line[0];
        val = stoi(line.substr(2));
        value.push_back(val);
        word.push_back(character);
        if (val > max)
            max = val;
    }

    int size = ceil_log(max + 1);

    std::string decrypt;
    std::cin >> decrypt;
    std::vector<std::string> binary_pieces;

    //Split the decrypt message into smaller pieces and put into a vector
    for (int i = 0; i < decrypt.length(); i += size)
    {
        binary_pieces.push_back(decrypt.substr(i, size));
    }

    // Find binary presentation in size of bit-width and then find the frequency, calling n_thread
    for (int i = 0; i < n_thread; i++)
    {
        arg[i].c = word[i];
        arg[i].dec_number = value[i];
        arg[i].bit_width = size;
        arg[i].ptr = &binary_pieces;
        if (pthread_create(&tid[i], NULL, decToBinary, &arg[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
        for (int i = 0; i < n_thread; i++)
            pthread_join(tid[i], NULL);
    }

    std::cout << "Alphabet:" << std::endl;
    std::map<std::string, char> charDict;
    //Put a key-value pair into a dictionary for decompressed message
    for (int i = 0; i < n_thread; i++)
    {
        charDict.insert(std::pair<std::string, char>(arg[i].binaryPresentation, arg[i].c));
        std::cout << "Character: " << arg[i].c << ", Code: " << arg[i].binaryPresentation << ", Frequency: " << arg[i].frequency << std::endl;
        if (i == n_thread - 1)
            std::cout << std::endl;
    }

    std::cout << "Decompressed message: ";
    // Create m_thread 
    int m_thread = decrypt.length() / size;
    pthread_t *tid_1 = new pthread_t[m_thread];
    struct message *mes = new message[m_thread];
    for (int i = 0; i < m_thread; i++)
    {
        mes[i].dictPtr = &charDict;
        mes[i].message = binary_pieces[i];
        if (pthread_create(&tid_1[i], NULL, decompressMessage, &mes[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
        for (int i = 0; i < m_thread; i++)
            pthread_join(tid_1[i], NULL);
    }
    for (int i = 0; i < m_thread; i++)
    {
        std::cout << &mes[i].c;
    }
    return 1;
}