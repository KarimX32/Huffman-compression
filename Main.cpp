#include <bits/stdc++.h>
using namespace std;

// Global buffer size
int bufferSize = 4096;

// Frequency node
struct FreqNode
{
    unsigned char byte;
    long long count;
    FreqNode *next;
};

// Tree node
struct TreeNode
{
    unsigned char byte;
    long long freq;
    TreeNode *left, *right;
};

// Queue node
struct QueueNode
{
    TreeNode *tree;
    QueueNode *next;
};

// Show help
void showHelp()
{
    cout << "How to use this program:" << endl;
    cout << "  app.exe -c input.txt [output.ece2103]      Compress a file" << endl;
    cout << "  app.exe -d input.txt.ece2103 [output.txt]  Decompress a file" << endl;
    cout << "  app.exe -b 8192                            Change buffer size to 8192 bytes" << endl;
    cout << "You can combine options like this:" << endl;
    cout << "  app.exe -b 16384 -c photo.png              Compress with 16KB buffer" << endl;
    cout << "Notes:" << endl;
    cout << "  - Square brackets [] indicate optional parameters" << endl;
    cout << "  - When decompressing, the original file type is restored automatically" << endl;
    cout << "  - Default buffer size = 4KB" << endl;
}

// Build frequency table
FreqNode *buildFreqTable(const char *fileName, long long &fileSize)
{
    FILE *file = fopen(fileName, "rb");
    if (!file)
        return NULL;

    long long freq[256] = {0};
    unsigned char buffer[65536];
    size_t bytesRead;
    fileSize = 0;

    while ((bytesRead = fread(buffer, 1, bufferSize, file)) > 0)
    {
        for (size_t i = 0; i < bytesRead; i++)
        {
            freq[buffer[i]]++;
        }
        fileSize += bytesRead;
    }

    fclose(file);

    // Build linked list
    FreqNode *head = NULL;
    for (int i = 0; i < 256; i++)
    {
        if (freq[i] > 0)
        {
            FreqNode *node = new FreqNode;
            node->byte = i;
            node->count = freq[i];
            node->next = head;
            head = node;
        }
    }

    return head;
}

// Bubble sort for queue
void sortQueue(QueueNode *head)
{
    if (!head)
        return;
    if (!head->next)
        return;

    bool sorted = false;
    while (!sorted)
    {
        sorted = true;
        QueueNode *curr = head;

        while (curr->next)
        {
            if (curr->tree->freq > curr->next->tree->freq)
            {
                TreeNode *temp = curr->tree;
                curr->tree = curr->next->tree;
                curr->next->tree = temp;
                sorted = false;
            }
            curr = curr->next;
        }
    }
}

// Add to queue
void addToQueue(QueueNode **head, TreeNode *tree)
{
    QueueNode *node = new QueueNode;
    node->tree = tree;
    node->next = NULL;

    if (!*head)
    {
        *head = node;
    }
    else
    {
        QueueNode *curr = *head;
        while (curr->next)
        {
            curr = curr->next;
        }
        curr->next = node;
    }

    sortQueue(*head);
}

// Remove from queue
TreeNode *removeFromQueue(QueueNode **head)
{
    if (!*head)
        return NULL;

    TreeNode *tree = (*head)->tree;
    QueueNode *temp = *head;
    *head = (*head)->next;
    delete temp;

    return tree;
}

// Build Huffman tree
TreeNode *buildTree(FreqNode *freqList)
{
    if (!freqList)
        return NULL;

    QueueNode *queue = NULL;

    // Add all frequencies to queue
    while (freqList)
    {
        TreeNode *leaf = new TreeNode;
        leaf->byte = freqList->byte;
        leaf->freq = freqList->count;
        leaf->left = NULL;
        leaf->right = NULL;

        addToQueue(&queue, leaf);
        freqList = freqList->next;
    }

    // Build tree
    while (queue && queue->next)
    {
        TreeNode *left = removeFromQueue(&queue);
        TreeNode *right = removeFromQueue(&queue);

        TreeNode *parent = new TreeNode;
        parent->byte = 0;
        parent->freq = left->freq + right->freq;
        parent->left = left;
        parent->right = right;

        addToQueue(&queue, parent);
    }

    if (queue)
    {
        return queue->tree;
    }
    else
    {
        return NULL;
    }
}

// Generate codes
void makeCodes(TreeNode *node, string code, string codes[256])
{
    if (!node)
        return;

    if (!node->left && !node->right)
    {
        if (code.empty())
        {
            codes[node->byte] = "0";
        }
        else
        {
            codes[node->byte] = code;
        }
        return;
    }

    makeCodes(node->left, code + "0", codes);
    makeCodes(node->right, code + "1", codes);
}

// Save codes
void saveCodes(string codes[256], const char *fileName)
{
    FILE *file = fopen(fileName, "wb");
    if (!file)
        return;

    for (int i = 0; i < 256; i++)
    {
        if (!codes[i].empty())
        {
            fprintf(file, "%d %s\n", i, codes[i].c_str());
        }
    }

    fclose(file);
}

// Load codes
TreeNode *loadCodes(const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    if (!file)
        return NULL;

    TreeNode *root = new TreeNode;
    root->byte = 0;
    root->freq = 0;
    root->left = NULL;
    root->right = NULL;

    int byte;
    char code[256];

    while (fscanf(file, "%d %s\n", &byte, code) == 2)
    {
        TreeNode *curr = root;

        for (int i = 0; code[i]; i++)
        {
            if (code[i] == '0')
            {
                if (!curr->left)
                {
                    curr->left = new TreeNode;
                    curr->left->byte = 0;
                    curr->left->freq = 0;
                    curr->left->left = NULL;
                    curr->left->right = NULL;
                }
                curr = curr->left;
            }
            else
            {
                if (!curr->right)
                {
                    curr->right = new TreeNode;
                    curr->right->byte = 0;
                    curr->right->freq = 0;
                    curr->right->left = NULL;
                    curr->right->right = NULL;
                }
                curr = curr->right;
            }
        }
        curr->byte = byte;
    }

    fclose(file);
    return root;
}

// Show progress
void showProgress(long long done, long long total, time_t start)
{
    if (total == 0)
        return;

    int percent = (done * 100) / total;
    time_t now = time(NULL);
    int elapsed = now - start;

    if (elapsed > 0 && done > 0)
    {
        int eta = (elapsed * total / done) - elapsed;
        cout << "\rProgress: " << percent << "% | ETA: " << eta << " seconds";
    }
}

// Compress file
void compress(const char *inFile, const char *outFile)
{
    // Build frequency table
    long long fileSize;
    FreqNode *freqs = buildFreqTable(inFile, fileSize);
    if (!freqs)
    {
        cout << "Sorry, there was a problem with this operation." << endl;
        return;
    }

    // Build tree
    TreeNode *root = buildTree(freqs);

    // Make codes
    string codes[256];
    makeCodes(root, "", codes);

    // Save codes
    string codeFile = string(outFile) + ".cod";
    saveCodes(codes, codeFile.c_str());

    // Open files
    FILE *input = fopen(inFile, "rb");
    FILE *output = fopen(outFile, "wb");
    if (!input || !output)
    {
        cout << "Sorry, there was a problem with this operation." << endl;
        return;
    }

    // Write file size
    fwrite(&fileSize, sizeof(long long), 1, output);

    // Compress data
    unsigned char buffer[65536];
    string bits = "";
    size_t bytesRead;
    long long done = 0;
    time_t start = time(NULL);

    while ((bytesRead = fread(buffer, 1, bufferSize, input)) > 0)
    {
        for (size_t i = 0; i < bytesRead; i++)
        {
            bits += codes[buffer[i]];

            while (bits.length() >= 8)
            {
                unsigned char byte = 0;
                for (int j = 0; j < 8; j++)
                {
                    byte = byte << 1;
                    if (bits[j] == '1')
                    {
                        byte = byte | 1;
                    }
                }
                fwrite(&byte, 1, 1, output);
                bits = bits.substr(8);
            }
        }
        done += bytesRead;
        showProgress(done, fileSize, start);
    }

    // Write last bits
    if (!bits.empty())
    {
        unsigned char byte = 0;
        for (size_t i = 0; i < bits.length(); i++)
        {
            byte = byte << 1;
            if (bits[i] == '1')
            {
                byte = byte | 1;
            }
        }
        byte = byte << (8 - bits.length());
        fwrite(&byte, 1, 1, output);
    }

    fclose(input);
    fclose(output);
    cout << "\nCompression complete!" << endl;
}

// Decompress file
void decompress(const char *inFile, const char *outFile)
{
    // Load tree
    string codeFile = string(inFile) + ".cod";
    TreeNode *root = loadCodes(codeFile.c_str());
    if (!root)
    {
        cout << "Sorry, there was a problem with this operation." << endl;
        return;
    }

    // Open files
    FILE *input = fopen(inFile, "rb");
    FILE *output = fopen(outFile, "wb");
    if (!input || !output)
    {
        cout << "Sorry, there was a problem with this operation." << endl;
        return;
    }

    // Read file size
    long long fileSize;
    fread(&fileSize, sizeof(long long), 1, input);

    // Decompress data
    unsigned char readBuf = new unsigned char[bufferSize];
    unsigned char writeBuf = new unsigned char[bufferSize];
    int writePos = 0;

    TreeNode *curr = root;
    size_t bytesRead;
    long long done = 0;
    time_t start = time(NULL);
    time_t lastUpdate = start;

    while ((bytesRead = fread(readBuf, 1, bufferSize, input)) > 0 && done < fileSize)
    {
        for (size_t i = 0; i < bytesRead && done < fileSize; i++)
        {
            for (int bit = 7; bit >= 0 && done < fileSize; bit--)
            {
                if ((readBuf[i] >> bit) & 1)
                {
                    if (curr->right)
                    {
                        curr = curr->right;
                    }
                }
                else
                {
                    if (curr->left)
                    {
                        curr = curr->left;
                    }
                }

                if (!curr->left && !curr->right)
                {
                    writeBuf[writePos] = curr->byte;
                    writePos++;
                    done++;
                    curr = root;

                    if (writePos >= bufferSize)
                    {
                        fwrite(writeBuf, 1, writePos, output);
                        writePos = 0;
                    }

                    time_t now = time(NULL);
                    if (now > lastUpdate)
                    {
                        showProgress(done, fileSize, start);
                        lastUpdate = now;
                    }
                }
            }
        }
    }

    // Write remaining
    if (writePos > 0)
    {
        fwrite(writeBuf, 1, writePos, output);
    }

    fclose(input);
    fclose(output);
    showProgress(done, fileSize, start);
    cout << "\nDecompression complete!" << endl;
}

// Main
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        showHelp();
        return 0;
    }

    bool doCompress = false;
    bool doDecompress = false;
    string inPath = "";
    string outPath = "";

    // Parse arguments
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];

        if (arg == "-c")
        {
            doCompress = true;
            if (i + 1 < argc)
            {
                inPath = argv[++i];
            }
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                outPath = argv[++i];
            }
        }
        else if (arg == "-d")
        {
            doDecompress = true;
            if (i + 1 < argc)
            {
                inPath = argv[++i];
            }
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                outPath = argv[++i];
            }
        }
        else if (arg == "-b" && i + 1 < argc)
        {
            bufferSize = atoi(argv[++i]);
        }
    }

    // Run operation
    if (doCompress && !inPath.empty())
    {
        if (outPath.empty())
        {
            outPath = inPath + ".ece2103";
        }
        compress(inPath.c_str(), outPath.c_str());
    }
    else if (doDecompress && !inPath.empty())
    {
        if (outPath.empty())
        {
            outPath = inPath;
            size_t pos = outPath.find(".ece2103");
            if (pos != string::npos)
            {
                outPath.erase(pos);
            }
        }
        decompress(inPath.c_str(), outPath.c_str());
    }
    else
    {
        cout << "Sorry, there was a problem with this operation." << endl;
    }

    return 0;
}