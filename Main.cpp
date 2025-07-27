// Moamen Eslam - 231004641 | Karim Amr - 231004157
#include <bits/stdc++.h>
using namespace std;

int BufferSize = 4096;

struct Node
{
    unsigned char byte;
    long long frequency;
    Node *left;
    Node *right;
    Node *next;

    Node(unsigned char b = 0, long long f = 0)
    {
        byte = b;
        frequency = f;
        left = right = next = nullptr;
    }
};

struct FreqList
{
    Node *head;

    FreqList() { head = nullptr; }

    void Insert(Node *node)
    {
        if (!head || node->frequency < head->frequency)
        {
            node->next = head;
            head = node;
        }
        else
        {
            Node *current = head;
            while (current->next != nullptr && current->next->frequency <= node->frequency)
                current = current->next;
            node->next = current->next;
            current->next = node;
        }
    }

    Node *RemoveMin()
    {
        if (!head)
            return nullptr;
        Node *minNode = head;
        head = head->next;
        minNode->next = nullptr;
        return minNode;
    }

    bool HasMoreThanOne()
    {
        return head && head->next;
    }
};

long long CharFrequency[256];
string HuffmanCodes[256];
Node *TreeRoot = nullptr;

long long GetFileSize(const char *filename);
void ShowProgress(long long processed, long long total, time_t startTime);

bool CountFrequencies(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return false;
    for (int i = 0; i < 256; i++)
    {
        CharFrequency[i] = 0;
    }
    unsigned char *buffer = new unsigned char[BufferSize];
    int BytesRead;
    while ((BytesRead = fread(buffer, 1, BufferSize, file)) > 0)
    {
        for (int i = 0; i < BytesRead; i++)
        {
            CharFrequency[buffer[i]]++;
        }
    }
    delete[] buffer;
    fclose(file);
    return true;
}

Node *BuildHuffmanTree()
{
    FreqList SortedList;
    for (int i = 0; i < 256; i++)
    {
        if (CharFrequency[i] > 0)
        {
            SortedList.Insert(new Node(i, CharFrequency[i]));
        }
    }
    if (!SortedList.head)
        return nullptr;
    if (!SortedList.head->next)
    {
        Node *root = new Node(0, SortedList.head->frequency);
        root->left = SortedList.head;
        return root;
    }
    while (SortedList.HasMoreThanOne())
    {
        Node *LeftNode = SortedList.RemoveMin();
        Node *RightNode = SortedList.RemoveMin();
        Node *ParentNode = new Node(0, LeftNode->frequency + RightNode->frequency);
        ParentNode->left = LeftNode;
        ParentNode->right = RightNode;
        SortedList.Insert(ParentNode);
    }
    return SortedList.RemoveMin();
}

void GenerateCodes(Node *root, string baseCode)
{
    if (!root)
        return;

    if (root->left == nullptr && root->right == nullptr)
    {
        if (baseCode.empty())
        {
            HuffmanCodes[root->byte] = "0";
        }
        else
        {
            HuffmanCodes[root->byte] = baseCode;
        }
        return;
    }

    if (root->left)
        GenerateCodes(root->left, baseCode + "0");
    if (root->right)
        GenerateCodes(root->right, baseCode + "1");
}

bool CompressFile(const char *InputFile, const char *OutputFile)
{
    if (!CountFrequencies(InputFile))
        return false;
    TreeRoot = BuildHuffmanTree();
    if (!TreeRoot)
        return false;
    for (int i = 0; i < 256; i++)
    {
        HuffmanCodes[i] = "";
    }
    GenerateCodes(TreeRoot, "");
    FILE *input = fopen(InputFile, "rb");
    FILE *output = fopen(OutputFile, "wb");
    if (!input || !output)
    {
        if (input)
            fclose(input);
        if (output)
            fclose(output);
        return false;
    }

    /*////////////// Got Help From AI Within The Next Section \\\\\\\\\\\\\\*/
    fwrite(CharFrequency, sizeof(long long), 256, output);

    for (int i = 0; i < 256; i++)
    {
        int len = HuffmanCodes[i].length();
        fwrite(&len, sizeof(int), 1, output);
        if (len > 0)
            fwrite(HuffmanCodes[i].c_str(), 1, len, output);
    }
    /*////////////// End of Assistance \\\\\\\\\\\\\\*/

    long long FileSize = GetFileSize(InputFile);
    long long processed = 0;
    time_t StartTime = time(nullptr);
    unsigned char *buffer = new unsigned char[BufferSize];
    string BitBuffer = "";
    int BytesRead;
    while ((BytesRead = fread(buffer, 1, BufferSize, input)) > 0)
    {

        for (int i = 0; i < BytesRead; i++)
        {
            BitBuffer += HuffmanCodes[buffer[i]];

            /*////////////// Got Help From AI Within The Next Section \\\\\\\\\\\\\\*/
            while (BitBuffer.length() >= 8)
            {
                unsigned char byte = 0;
                for (int j = 0; j < 8; j++)
                {                              
                    if (BitBuffer[j] == '1')  
                        byte += (128 >> j);   
                }
                fwrite(&byte, 1, 1, output);       
                BitBuffer = BitBuffer.substr(8);
            }
            /*////////////// End of Assistance \\\\\\\\\\\\\\*/
        }
        processed += BytesRead;
        ShowProgress(processed, FileSize, StartTime);
    }
    if (!BitBuffer.empty())   
    {
        unsigned char byte = 0;
        for (int j = 0; j < BitBuffer.length(); j++)
        {
            if (BitBuffer[j] == '1')
                byte += (128 >> j);
        }
        fwrite(&byte, 1, 1, output);
    }
    delete[] buffer;
    fclose(input);
    fclose(output);
    cout << endl;
    return true;
}

bool DecompressFile(const char *InputFile, const char *OutputFile)
{
    FILE *input = fopen(InputFile, "rb");
    FILE *output = fopen(OutputFile, "wb");
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
        if (fread(&CharFrequency[i], sizeof(long long), 1, input) != 1)
        {
            fclose(input);
            fclose(output);
            return false;
        }
    }
    for (int i = 0; i < 256; i++)
    {
        int CodeLength;
        if (fread(&CodeLength, sizeof(int), 1, input) != 1)
        {
            fclose(input);
            fclose(output);
            return false;
        }
        if (CodeLength > 0)
        {
            char CodeString[256];
            if (CodeLength >= 256)
            {
                fclose(input);
                fclose(output);
                return false;
            }
            if (fread(CodeString, 1, CodeLength, input) != CodeLength)
            {
                fclose(input);
                fclose(output);
                return false;
            }
            CodeString[CodeLength] = '\0';
            HuffmanCodes[i] = CodeString;
        }
        else
        {
            HuffmanCodes[i] = "";
        }
    }
    TreeRoot = BuildHuffmanTree();
    if (!TreeRoot)
    {
        fclose(input);
        fclose(output);
        return true;
    }
    long long TotalBytes = 0;
    for (int i = 0; i < 256; i++)
    {
        TotalBytes += CharFrequency[i];
    }
    if (TotalBytes == 0)
    {
        fclose(input);
        fclose(output);
        return true;
    }
    long long FileSize = GetFileSize(InputFile);
    long long processed = 0;
    time_t StartTime = time(nullptr);
    unsigned char *buffer = new unsigned char[BufferSize];
    Node *current = TreeRoot;
    long long BytesWritten = 0;
    int BytesRead;
    while (BytesWritten < TotalBytes)
    {
        BytesRead = fread(buffer, 1, BufferSize, input);
        if (BytesRead <= 0)
            break;

        for (int bufferIndex = 0; bufferIndex < BytesRead && BytesWritten < TotalBytes; bufferIndex++)
        {
            unsigned char InputByte = buffer[bufferIndex];
            /*////////////// Got Help From AI Within The Next Section \\\\\\\\\\\\\\*/
            for (int bit = 7; bit >= 0 && BytesWritten < TotalBytes; bit--)
            {
                if ((InputByte >> bit) & 1) 
                    current = current->right;
                else
                    current = current->left;

                if (current->left == nullptr && current->right == nullptr)
                {
                    fwrite(&current->byte, 1, 1, output);
                    BytesWritten++;
                    current = TreeRoot;
                }
            }
            /*////////////// End of Assistance \\\\\\\\\\\\\\*/
        }
        processed += BytesRead;
        ShowProgress(processed, FileSize, StartTime);
    }
    delete[] buffer;
    fclose(input);
    fclose(output);
    cout << endl;
    return true;
}

void ShowError()
{
    cout << "Sorry, this operation did not work." << endl;
}

void ShowHelp()
{
    cout << "How to use this program:" << endl;
    cout << "  app.exe -c input.txt      Compress a file" << endl;
    cout << "  app.exe -d input.ece2103  Decompress a file" << endl;
    cout << "  app.exe -b 8192           Change buffer size to 8192 bytes" << endl;
    cout << "You can combine options like this:" << endl;
    cout << "  app.exe -b 16384 -c photo.png              Compress with 16KB buffer" << endl;
    cout << "Note: The default buffer size is 4KB." << endl;
}

string GetDecompressedFilename(const char *CompressedFile)
{
    string filename = CompressedFile;
    int len = filename.length();
    if (len > 8 && filename.substr(len - 8) == ".ece2103")
        return filename.substr(0, len - 8);
    return filename + ".decompressed";
}

long long GetFileSize(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return 0;

    unsigned char buffer[1024];
    long long TotalSize = 0;
    int BytesRead;

    while ((BytesRead = fread(buffer, 1, 1024, file)) > 0)
    {
        TotalSize += BytesRead;
    }

    fclose(file);
    return TotalSize;
}

void ShowProgress(long long processed, long long total, time_t StartTime)
{
    int percent = (processed * 100) / total;
    time_t elapsed = time(nullptr) - StartTime;

    if (elapsed > 0 && processed > 0)
    {
        int eta = (int)((total - processed) * elapsed / processed);
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
        ShowHelp();
        return 0;
    }

    bool compress = false, decompress = false;
    string InputFile = "";

    for (int i = 1; i < argc; i++)
    {
        if (string(argv[i]) == "-c" && i + 1 < argc)
        {
            compress = true;
            InputFile = argv[++i];
        }
        else if (string(argv[i]) == "-d" && i + 1 < argc)
        {
            decompress = true;
            InputFile = argv[++i];
        }
        else if (string(argv[i]) == "-b" && i + 1 < argc)
        {
            BufferSize = atoi(argv[++i]);
            if (BufferSize <= 0)
                BufferSize = 4096;
        }
    }

    if (compress && !InputFile.empty())
    {
        string OutputFile = InputFile + ".ece2103";
        if (!CompressFile(InputFile.c_str(), OutputFile.c_str()))
        {
            ShowError();
            return 1;
        }
    }
    else if (decompress && !InputFile.empty())
    {
        string OutputFile = GetDecompressedFilename(InputFile.c_str());
        if (!DecompressFile(InputFile.c_str(), OutputFile.c_str()))
        {
            ShowError();
            return 1;
        }
    }
    else
    {
        ShowHelp();
    }

    return 0;
}