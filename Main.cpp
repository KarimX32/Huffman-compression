#include <bits/stdc++.h>
using namespace std;

struct FrequencyTable
{
    int counts[256];
    int totalBytes;

    FrequencyTable()
    {
        for (int i = 0; i < 256; i++)
            counts[i] = 0;
        totalBytes = 0;
    }

    void CountByte(char byte)
    {
        counts[(unsigned char)byte]++;
        totalBytes++;
    }

    int GetFrequency(char byte)
    {
        return counts[(unsigned char)byte];
    }

    bool HasCharacter(char byte)
    {
        return counts[(unsigned char)byte] > 0;
    }
};

struct TreeNode
{
    char character;
    int frequency;
    TreeNode *left;
    TreeNode *right;

    TreeNode(char ch, int freq)
    {
        character = ch;
        frequency = freq;
        left = NULL;
        right = NULL;
    }

    bool IsLeaf()
    {
        return left == NULL && right == NULL;
    }
};

struct QueueNode
{
    TreeNode *data;
    QueueNode *next;

    QueueNode(TreeNode *node)
    {
        data = node;
        next = NULL;
    }
};

struct PriorityQueue
{
    QueueNode *head;
    int size;

    PriorityQueue()
    {
        head = NULL;
        size = 0;
    }

    void Insert(TreeNode *node)
    {
        QueueNode *newNode = new QueueNode(node);
        size++;

        if (head == NULL || node->frequency < head->data->frequency)
        {
            newNode->next = head;
            head = newNode;
            return;
        }

        QueueNode *current = head;
        while (current->next != NULL && current->next->data->frequency <= node->frequency)
        {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;
    }

    TreeNode *RemoveMin()
    {
        if (head == NULL)
            return NULL;

        QueueNode *temp = head;
        TreeNode *node = temp->data;
        head = head->next;
        size--;

        return node;
    }

    int GetSize()
    {
        return size;
    }
};

struct HuffmanTree
{
    TreeNode *root;
    string originalFileName;

    HuffmanTree()
    {
        root = NULL;
    }

    HuffmanTree(FrequencyTable *table)
    {
        BuildTree(table);
    }

    void BuildTree(FrequencyTable *table)
    {
        PriorityQueue queue;

        for (int i = 0; i < 256; i++)
        {
            char ch = (char)i;
            if (table->HasCharacter(ch))
            {
                TreeNode *node = new TreeNode(ch, table->GetFrequency(ch));
                queue.Insert(node);
            }
        }

        if (queue.GetSize() == 1)
        {
            TreeNode *singleNode = queue.RemoveMin();
            root = new TreeNode(0, singleNode->frequency);
            root->left = singleNode;
            return;
        }

        while (queue.GetSize() > 1)
        {
            TreeNode *left = queue.RemoveMin();
            TreeNode *right = queue.RemoveMin();

            int combinedFreq = left->frequency + right->frequency;
            TreeNode *parent = new TreeNode(0, combinedFreq);
            parent->left = left;
            parent->right = right;

            queue.Insert(parent);
        }

        root = queue.RemoveMin();
    }

    TreeNode *GetRoot()
    {
        return root;
    }

    string GetOriginalFileName()
    {
        return originalFileName;
    }

    void SaveTreeNode(TreeNode *node, ofstream &file)
    {
        if (node == NULL)
            return;

        if (node->IsLeaf())
        {
            file.put(0);
            file.put(node->character);
        }
        else
        {
            file.put(1);
            SaveTreeNode(node->left, file);
            SaveTreeNode(node->right, file);
        }
    }

    void SaveToFile(ofstream &file, string fileName)
    {
        int nameLength = fileName.length();
        file << nameLength << endl;
        file << fileName << endl;
        SaveTreeNode(root, file);
    }

    TreeNode *LoadTreeNode(ifstream &file)
    {
        if (file.eof())
            return NULL;

        char nodeType;
        file.get(nodeType);

        if (nodeType == 0)
        {
            char ch;
            file.get(ch);
            return new TreeNode(ch, 0);
        }
        else
        {
            TreeNode *node = new TreeNode(0, 0);
            node->left = LoadTreeNode(file);
            node->right = LoadTreeNode(file);
            return node;
        }
    }

    bool LoadFromFile(ifstream &file)
    {
        int nameLength;
        file >> nameLength;
        file.ignore();

        char nameBuffer[1000];
        file.getline(nameBuffer, 1000);
        originalFileName = nameBuffer;

        root = LoadTreeNode(file);
        return root != NULL;
    }

    void GenerateCodes(TreeNode *node, BitPattern currentCode, CodeBook *table);
    CodeBook *CreateCodeBook();
};

struct BitPattern
{
    bool bits[256];
    int length;

    BitPattern()
    {
        length = 0;
    }
};

struct CodeBook
{
    BitPattern codes[256];
    bool hasCode[256];

    CodeBook()
    {
        for (int i = 0; i < 256; i++)
            hasCode[i] = false;
    }

    void SetCodeFor(char byte, BitPattern code)
    {
        codes[(unsigned char)byte] = code;
        hasCode[(unsigned char)byte] = true;
    }

    BitPattern GetCodeFor(char byte)
    {
        return codes[(unsigned char)byte];
    }
};

void HuffmanTree::GenerateCodes(TreeNode *node, BitPattern currentCode, CodeBook *table)
{
    if (node == NULL)
        return;

    if (node->IsLeaf())
    {
        table->SetCodeFor(node->character, currentCode);
        return;
    }

    BitPattern leftCode = currentCode;
    leftCode.bits[leftCode.length] = false;
    leftCode.length++;
    GenerateCodes(node->left, leftCode, table);

    BitPattern rightCode = currentCode;
    rightCode.bits[rightCode.length] = true;
    rightCode.length++;
    GenerateCodes(node->right, rightCode, table);
}

CodeBook *HuffmanTree::CreateCodeBook()
{
    CodeBook *table = new CodeBook();
    BitPattern emptyCode;
    emptyCode.length = 0;

    GenerateCodes(root, emptyCode, table);
    return table;
}

struct FileCompressor
{
    int bufferSize;

    FileCompressor(int size)
    {
        bufferSize = size;
    }

    FrequencyTable *AnalyzeFile(string fileName)
    {
        ifstream file(fileName);
        if (file.is_open() == false)
        {
            return NULL;
        }

        FrequencyTable *table = new FrequencyTable();
        char *buffer = new char[bufferSize];

        while (file.eof() == false)
        {
            file.read(buffer, bufferSize);
            int bytesRead = file.gcount();
            if (bytesRead <= 0)
                break;

            for (int i = 0; i < bytesRead; i++)
            {
                table->CountByte(buffer[i]);
            }
        }

        file.close();
        return table;
    }

    bool CompressFile(string inputFileName, string outputFileName, HuffmanTree *tree, CodeBook *codes)
    {
        ifstream inputFile(inputFileName);
        if (inputFile.is_open() == false)
        {
            return false;
        }

        ofstream outputFile(outputFileName);
        if (outputFile.is_open() == false)
        {
            inputFile.close();
            return false;
        }

        outputFile << "ECE2103HUFFMAN" << endl;
        tree->SaveToFile(outputFile, inputFileName);

        char *buffer = new char[bufferSize];
        char currentByte = 0;
        int bitsWritten = 0;

        while (inputFile.eof() == false)
        {
            inputFile.read(buffer, bufferSize);
            int bytesRead = inputFile.gcount();
            if (bytesRead <= 0)
                break;

            for (int i = 0; i < bytesRead; i++)
            {
                char ch = buffer[i];
                BitPattern code = codes->GetCodeFor(ch);

                for (int j = 0; j < code.length; j++)
                {
                    if (code.bits[j] == true)
                    {
                        currentByte = currentByte | (1 << (7 - bitsWritten));
                    }

                    bitsWritten++;

                    if (bitsWritten == 8)
                    {
                        outputFile.put(currentByte);
                        currentByte = 0;
                        bitsWritten = 0;
                    }
                }
            }
        }

        if (bitsWritten > 0)
        {
            outputFile.put(currentByte);
        }

        inputFile.close();
        outputFile.close();
        return true;
    }

    bool Compress(string inputFileName, string outputFileName)
    {
        if (outputFileName.empty())
        {
            outputFileName = inputFileName + ".ece2103";
        }

        FrequencyTable *table = AnalyzeFile(inputFileName);
        if (table == NULL)
            return false;

        HuffmanTree *tree = new HuffmanTree(table);

        CodeBook *codes = tree->CreateCodeBook();

        bool success = CompressFile(inputFileName, outputFileName, tree, codes);
        return success;
    }

    bool DecompressFile(string inputFileName, string outputFileName)
    {
        ifstream inputFile(inputFileName);
        if (inputFile.is_open() == false)
        {
            return false;
        }

        string actualOutputFileName = outputFileName;
        if (outputFileName.empty())
        {
            char header[16];
            inputFile.getline(header, 16);

            if (strcmp(header, "ECE2103HUFFMAN") != 0)
            {
                inputFile.close();
                return false;
            }

            int nameLength;
            inputFile.read((char *)&nameLength, sizeof(int));

            char *nameBuffer = new char[nameLength + 1];
            inputFile.read(nameBuffer, nameLength);
            nameBuffer[nameLength] = '\0';
            actualOutputFileName = nameBuffer;

            inputFile.clear();
            inputFile.seekg(0);
        }

        ofstream outputFile(actualOutputFileName);
        if (outputFile.is_open() == false)
        {
            inputFile.close();
            return false;
        }

        string header;
        getline(inputFile, header);
        if (header != "ECE2103HUFFMAN")
        {
            return false;
        }

        HuffmanTree *tree = new HuffmanTree();
        if (tree->LoadFromFile(inputFile) == false)
        {
            return false;
        }

        if (outputFileName.empty())
        {
            string originalFileName = tree->GetOriginalFileName();
        }

        TreeNode *currentNode = tree->GetRoot();
        char *buffer = new char[bufferSize];

        while (inputFile.eof() == false)
        {
            inputFile.read(buffer, bufferSize);
            int bytesRead = inputFile.gcount();
            if (bytesRead <= 0)
                break;

            for (int i = 0; i < bytesRead; i++)
            {
                char byte = buffer[i];

                for (int bitPosition = 7; bitPosition >= 0; bitPosition--)
                {
                    bool bit = false;
                    if ((byte & (1 << bitPosition)) != 0)
                        bit = true;

                    if (bit == true)
                        currentNode = currentNode->right;
                    else
                        currentNode = currentNode->left;

                    if (currentNode->IsLeaf())
                    {
                        outputFile.put(currentNode->character);
                        currentNode = tree->GetRoot();
                    }
                }
            }
        }

        inputFile.close();
        outputFile.close();
        return true;
    }

    bool Decompress(string inputFileName, string outputFileName)
    {
        return DecompressFile(inputFileName, outputFileName);
    }
};

void ShowHelp()
{
    cout << "Huffman File Compressor - How to use:" << endl;
    cout << "  app.exe -c input.txt [output.ece2103]      Compress a file" << endl;
    cout << "  app.exe -d input.ece2103 [output.txt]      Decompress a file" << endl;
    cout << "  app.exe -b 8192                            Set buffer size (default: 4096 bytes)" << endl;
    cout << "Examples:" << endl;
    cout << "  app.exe -c document.pdf                     Compress to document.pdf.ece2103" << endl;
    cout << "  app.exe -d document.pdf.ece2103             Restore original document.pdf" << endl;
    cout << "Notes:" << endl;
    cout << "  - Output filenames in [] are optional" << endl;
    cout << "  - Original filename is preserved automatically" << endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        ShowHelp();
        return 1;
    }

    bool wantToCompress = false;
    bool wantToDecompress = false;
    int bufferSize = 4096;
    string inputFileName = "";
    string outputFileName = "";

    for (int i = 1; i < argc; i++)
    {
        string option = argv[i];

        if (option == "-c")
        {
            wantToCompress = true;
            if (i + 1 < argc)
            {
                inputFileName = argv[i + 1];
                i++;
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    outputFileName = argv[i + 1];
                    i++;
                }
            }
            else
            {
                cout << "No file specified" << endl;
                return 1;
            }
        }
        else if (option == "-d")
        {
            wantToDecompress = true;
            if (i + 1 < argc)
            {
                inputFileName = argv[i + 1];
                i++;
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    outputFileName = argv[i + 1];
                    i++;
                }
            }
            else
            {
                cout << "No file specified" << endl;
                return 1;
            }
        }
        else if (option == "-b" && i + 1 < argc)
        {
            bufferSize = stoi(argv[i + 1]);
            i++;
        }
        else
        {
            return 1;
        }
    }

    FileCompressor compressor(bufferSize);
    clock_t startTime = clock();
    bool success = false;

    if (wantToCompress)
    {
        success = compressor.Compress(inputFileName, outputFileName);
    }
    else if (wantToDecompress)
    {
        success = compressor.Decompress(inputFileName, outputFileName);
    }
    else
    {
        cout << "Use -c or -d" << endl;
        return 1;
    }

    clock_t endTime = clock();
    double elapsedTime = double(endTime - startTime) / CLOCKS_PER_SEC;

    if (success)
    {
        cout << "Done in " << elapsedTime << " seconds" << endl;
        return 0;
    }
    else
    {
        cout << "Failed" << endl;
        return 1;
    }
}

struct ProgressBar
{
    long totalSize;
    long processedSize;
    time_t startTime;
    string taskName;

    ProgressBar()
    {
        totalSize = 0;
        processedSize = 0;
    }

    void SetTotal(long bytes)
    {
        totalSize = bytes;
    }

    void SetTask(string name)
    {
        taskName = name;
    }

    void Start()
    {
        startTime = time(NULL);
        processedSize = 0;
        cout << taskName << " started" << endl;
    }

    int EstimateTimeLeft()
    {
        if (processedSize == 0)
            return 0;

        time_t elapsed = time(NULL) - startTime;
        if (elapsed == 0)
            return 0;

        double speed = (double)processedSize / elapsed;
        if (speed == 0)
            return 0;

        return (int)((totalSize - processedSize) / speed);
    }

    void Display()
    {
        if (totalSize <= 0)
            return;

        int percentage = (int)((double)processedSize / totalSize * 100);
        int timeLeft = EstimateTimeLeft();

        cout << "\r" << setw(70) << " " << "\r";
        cout << taskName << ": " << percentage << "% done";

        if (timeLeft > 0)
        {
            cout << " (About ";
            if (timeLeft >= 3600)
            {
                cout << timeLeft / 3600 << "h ";
                timeLeft %= 3600;
            }
            if (timeLeft >= 60)
            {
                cout << timeLeft / 60 << "m ";
                timeLeft %= 60;
            }
            cout << timeLeft << "s left)";
        }

        cout << flush;
    }

    void UpdateProgress(long done)
    {
        processedSize = done;

        static time_t lastUpdate = 0;
        static long lastDone = 0;

        time_t now = time(NULL);
        bool shouldUpdate = (now - lastUpdate) >= 1;
        bool goodProgress = (processedSize - lastDone) > (totalSize / 100);

        if (shouldUpdate || goodProgress)
        {
            Display();
            lastUpdate = now;
            lastDone = processedSize;
        }
    }

    void Complete()
    {
        UpdateProgress(totalSize);
        cout << endl
             << taskName << " completed in ";

        time_t totalTime = time(NULL) - startTime;
        if (totalTime >= 3600)
        {
            cout << totalTime / 3600 << "h ";
            totalTime %= 3600;
        }
        if (totalTime >= 60)
        {
            cout << totalTime / 60 << "m ";
            totalTime %= 60;
        }
        cout << totalTime << "s" << endl;
    }
};