#include <bits/stdc++.h>
using namespace std;

int BUFFER_SIZE = 4096;

struct Node
{
    unsigned char byte;
    long long freq;
    Node *left;
    Node *right;
    Node *next;

    Node(unsigned char b = 0, long long f = 0)
    {
        byte = b;
        freq = f;
        left = right = next = nullptr;
    }
};

struct FreqList
{
    Node *head;

    FreqList() { head = nullptr; }

    void insert(Node *node)
    {
        if (!head || node->freq < head->freq)
        {
            node->next = head;
            head = node;
            return;
        }

        Node *curr = head;
        while (curr->next && curr->next->freq <= node->freq)
        {
            curr = curr->next;
        }
        node->next = curr->next;
        curr->next = node;
    }

    Node *removeMin()
    {
        if (!head)
            return nullptr;
        Node *min_node = head;
        head = head->next;
        min_node->next = nullptr;
        return min_node;
    }

    bool hasMoreThanOne()
    {
        return head && head->next;
    }
};

long long frequency[256];
string huffmanCodes[256];
Node *huffmanRoot = nullptr;

long long getFileSize(const char *filename);
void showProgress(long long processed, long long total, time_t startTime);

bool countFrequencies(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return false;
    for (int i = 0; i < 256; i++)
    {
        frequency[i] = 0;
    }
    unsigned char *buffer = new unsigned char[BUFFER_SIZE];
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        for (int i = 0; i < bytesRead; i++)
        {
            frequency[buffer[i]]++;
        }
    }
    delete[] buffer;
    fclose(file);
    return true;
}

Node *buildHuffmanTree()
{
    FreqList freqList;
    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            freqList.insert(new Node(i, frequency[i]));
        }
    }
    if (!freqList.head)
        return nullptr;
    if (!freqList.head->next)
    {
        Node *root = new Node(0, freqList.head->freq);
        root->left = freqList.head;
        return root;
    }
    while (freqList.hasMoreThanOne())
    {
        Node *left = freqList.removeMin();
        Node *right = freqList.removeMin();
        Node *parent = new Node(0, left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        freqList.insert(parent);
    }
    return freqList.removeMin();
}

void generateCodes(Node *node, string code)
{
    if (!node)
        return;
    if (!node->left && !node->right)
    {
        huffmanCodes[node->byte] = code.empty() ? "0" : code;
        return;
    }
    generateCodes(node->left, code + "0");
    generateCodes(node->right, code + "1");
}

bool compressFile(const char *inputFile, const char *outputFile)
{
    if (!countFrequencies(inputFile))
        return false;
    huffmanRoot = buildHuffmanTree();
    if (!huffmanRoot)
        return false;
    for (int i = 0; i < 256; i++)
    {
        huffmanCodes[i] = "";
    }
    generateCodes(huffmanRoot, "");
    FILE *input = fopen(inputFile, "rb");
    FILE *output = fopen(outputFile, "wb");
    if (!input || !output)
    {
        if (input)
            fclose(input);
        if (output)
            fclose(output);
        return false;
    }
    for (int i = 0; i < 256; i++)
    {
        fwrite(&frequency[i], sizeof(long long), 1, output);
    }
    for (int i = 0; i < 256; i++)
    {
        int codeLength = huffmanCodes[i].length();
        fwrite(&codeLength, sizeof(int), 1, output);
        if (codeLength > 0)
        {
            fwrite(huffmanCodes[i].c_str(), sizeof(char), codeLength, output);
        }
    }
    long long fileSize = getFileSize(inputFile);
    long long processed = 0;
    time_t startTime = time(nullptr);
    unsigned char *buffer = new unsigned char[BUFFER_SIZE];
    string bitBuffer = "";
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, input)) > 0)
    {
        for (int i = 0; i < bytesRead; i++)
        {
            bitBuffer += huffmanCodes[buffer[i]];

            while (bitBuffer.length() >= 8)
            {
                unsigned char outputByte = 0;
                for (int j = 0; j < 8; j++)
                {
                    if (bitBuffer[j] == '1')
                    {
                        outputByte = outputByte + (1 << (7 - j));
                    }
                }
                fwrite(&outputByte, 1, 1, output);
                bitBuffer = bitBuffer.substr(8);
            }
        }
        processed += bytesRead;
        showProgress(processed, fileSize, startTime);
    }
    if (!bitBuffer.empty())
    {
        unsigned char outputByte = 0;
        for (int j = 0; j < bitBuffer.length(); j++)
        {
            if (bitBuffer[j] == '1')
            {
                outputByte = outputByte + (1 << (7 - j));
            }
        }
        fwrite(&outputByte, 1, 1, output);
    }
    delete[] buffer;
    fclose(input);
    fclose(output);
    cout << endl;
    return true;
}

bool decompressFile(const char *inputFile, const char *outputFile)
{
    FILE *input = fopen(inputFile, "rb");
    FILE *output = fopen(outputFile, "wb");
    if (!input || !output)
    {
        if (input)
            fclose(input);
        if (output)
            fclose(output);
        return false;
    }
    for (int i = 0; i < 256; i++)
    {
        if (fread(&frequency[i], sizeof(long long), 1, input) != 1)
        {
            fclose(input);
            fclose(output);
            return false;
        }
    }
    for (int i = 0; i < 256; i++)
    {
        int codeLength;
        if (fread(&codeLength, sizeof(int), 1, input) != 1)
        {
            fclose(input);
            fclose(output);
            return false;
        }
        if (codeLength > 0)
        {
            char *codeStr = new char[codeLength + 1];
            if (fread(codeStr, sizeof(char), codeLength, input) != codeLength)
            {
                delete[] codeStr;
                fclose(input);
                fclose(output);
                return false;
            }
            codeStr[codeLength] = '\0';
            huffmanCodes[i] = string(codeStr);
            delete[] codeStr;
        }
        else
        {
            huffmanCodes[i] = "";
        }
    }
    huffmanRoot = buildHuffmanTree();
    if (!huffmanRoot)
    {
        fclose(input);
        fclose(output);
        return true;
    }
    long long totalBytes = 0;
    for (int i = 0; i < 256; i++)
    {
        totalBytes += frequency[i];
    }
    if (totalBytes == 0)
    {
        fclose(input);
        fclose(output);
        return true;
    }
    long long fileSize = getFileSize(inputFile);
    long long processed = 0;
    time_t startTime = time(nullptr);
    unsigned char *buffer = new unsigned char[BUFFER_SIZE];
    Node *current = huffmanRoot;
    long long bytesWritten = 0;
    int bytesRead;
    while (bytesWritten < totalBytes && (bytesRead = fread(buffer, 1, BUFFER_SIZE, input)) > 0)
    {
        for (int i = 0; i < bytesRead && bytesWritten < totalBytes; i++)
        {
            unsigned char inputByte = buffer[i];
            for (int bit = 7; bit >= 0 && bytesWritten < totalBytes; bit--)
            {
                if (inputByte & (1 << bit))
                {
                    current = current->right;
                }
                else
                {
                    current = current->left;
                }

                if (current->left == nullptr && current->right == nullptr)
                {
                    fwrite(&current->byte, 1, 1, output);
                    bytesWritten++;
                    current = huffmanRoot;
                }
            }
        }
        processed += bytesRead;
        showProgress(processed, fileSize, startTime);
    }
    delete[] buffer;
    fclose(input);
    fclose(output);
    cout << endl;
    return true;
}

void showError()
{
    cout << "Sorry, this operation did not work." << endl;
}

void showHelp()
{
    cout << "How to use this program:" << endl;
    cout << "  app.exe -c input.txt      Compress a file" << endl;
    cout << "  app.exe -d input.ece2103  Decompress a file" << endl;
    cout << "  app.exe -b 8192           Change buffer size to 8192 bytes" << endl;
    cout << "You can combine options like this:" << endl;
    cout << "  app.exe -b 16384 -c photo.png              Compress with 16KB buffer" << endl;
}

string getDecompressedFilename(const char *compressedFile)
{
    string filename = string(compressedFile);
    if (filename.length() > 8 && filename.substr(filename.length() - 8) == ".ece2103")
    {
        return filename.substr(0, filename.length() - 8);
    }
    return filename + ".decompressed";
}

long long getFileSize(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return 0;

    unsigned char buffer[1024];
    long long totalSize = 0;
    int bytesRead;

    while ((bytesRead = fread(buffer, 1, 1024, file)) > 0)
    {
        totalSize += bytesRead;
    }

    fclose(file);
    return totalSize;
}

void showProgress(long long processed, long long total, time_t startTime)
{
    int percent = (processed * 100) / total;
    time_t currentTime = time(nullptr);
    double elapsed = difftime(currentTime, startTime);

    if (elapsed > 0 && processed > 0)
    {
        double rate = processed / elapsed;
        long long remaining = total - processed;
        int eta = (int)(remaining / rate);
        int hours = eta / 3600;
        int minutes = (eta % 3600) / 60;
        int seconds = eta % 60;
        printf("\r%d%% | ETA: %02d:%02d:%02d", percent, hours, minutes, seconds);
    }
    else
    {
        printf("\r%d%% | ETA: 00:00:00", percent);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        showHelp();
        return 0;
    }
    bool compress = false, decompress = false;
    string inputFile = "";
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "-c")
        {
            compress = true;
            if (i + 1 < argc)
            {
                inputFile = argv[++i];
            }
        }
        else if (arg == "-d")
        {
            decompress = true;
            if (i + 1 < argc)
            {
                inputFile = argv[++i];
            }
        }
        else if (arg == "-b")
        {
            if (i + 1 < argc)
            {
                BUFFER_SIZE = atoi(argv[++i]);
                if (BUFFER_SIZE <= 0)
                    BUFFER_SIZE = 4096;
            }
        }
    }
    if (compress && !inputFile.empty())
    {
        string outputFile = inputFile + ".ece2103";
        if (!compressFile(inputFile.c_str(), outputFile.c_str()))
        {
            showError();
            return 1;
        }
    }
    else if (decompress && !inputFile.empty())
    {
        string outputFile = getDecompressedFilename(inputFile.c_str());
        if (!decompressFile(inputFile.c_str(), outputFile.c_str()))
        {
            showError();
            return 1;
        }
    }
    else
    {
        showHelp();
    }
    return 0;
}