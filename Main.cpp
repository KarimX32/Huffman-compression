#include <bits/stdc++.h>
using namespace std;

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
    bool byteMapped[256];

    CodeBook()
    {
        memset(byteMapped, 0, sizeof(byteMapped));
    }

    void setCodeFor(unsigned char byte, BitPattern code)
    {
        codes[byte] = code;
        byteMapped[byte] = true;
    }

    BitPattern getCodeFor(unsigned char byte)
    {
        return codes[byte];
    }

    bool hasCodeFor(unsigned char byte)
    {
        return byteMapped[byte];
    }
};

struct HuffmanTreeNode
{
    unsigned char symbol;
    unsigned int count;
    HuffmanTreeNode *left;
    HuffmanTreeNode *right;

    HuffmanTreeNode(unsigned char byte, unsigned int frequency)
    {
        symbol = byte;
        count = frequency;
        left = NULL;
        right = NULL;
    }

    ~HuffmanTreeNode()
    {
        delete left;
        delete right;
    }

    bool isLeaf()
    {
        return left == NULL && right == NULL;
    }
};

struct QueueNode
{
    HuffmanTreeNode *data;
    QueueNode *next;

    QueueNode(HuffmanTreeNode *node)
    {
        data = node;
        next = NULL;
    }
};

struct PriorityQueue
{
    QueueNode *head;
    int nodeCount;

    PriorityQueue()
    {
        head = NULL;
        nodeCount = 0;
    }

    ~PriorityQueue()
    {
        QueueNode *current = head;
        while (current != NULL)
        {
            QueueNode *temp = current;
            current = current->next;
            delete temp;
        }
    }

    void add(HuffmanTreeNode *node)
    {
        QueueNode *newNode = new QueueNode(node);
        nodeCount++;

        if (head == NULL || node->count < head->data->count)
        {
            newNode->next = head;
            head = newNode;
            return;
        }

        QueueNode *current = head;
        while (current->next != NULL &&
               current->next->data->count <= node->count)
        {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;
    }

    HuffmanTreeNode *getNext()
    {
        if (head == NULL)
            return NULL;

        QueueNode *temp = head;
        HuffmanTreeNode *node = temp->data;
        head = head->next;
        delete temp;
        nodeCount--;

        return node;
    }

    bool isEmpty()
    {
        return head == NULL;
    }

    int size()
    {
        return nodeCount;
    }
};

struct FrequencyCounter
{
    unsigned int counts[256];
    unsigned int totalBytes;

    FrequencyCounter()
    {
        memset(counts, 0, sizeof(counts));
        totalBytes = 0;
    }

    void addByte(unsigned char byte)
    {
        counts[byte]++;
        totalBytes++;
    }

    unsigned int getCount(unsigned char byte)
    {
        return counts[byte];
    }

    unsigned int getTotalBytes()
    {
        return totalBytes;
    }

    bool hasByte(unsigned char byte)
    {
        return counts[byte] > 0;
    }
};

struct ProgressBar
{
    long totalSize;
    long doneSize;
    time_t startTime;
    string taskName;

    ProgressBar()
    {
        totalSize = 0;
        doneSize = 0;
    }

    void setTotalSize(long bytes)
    {
        totalSize = bytes;
    }

    void setTaskName(string name)
    {
        taskName = name;
    }

    void start()
    {
        startTime = time(NULL);
        doneSize = 0;
        cout << taskName << " started..." << endl;
    }

    int calculateTimeLeft()
    {
        if (doneSize == 0)
            return 0;

        time_t timeSoFar = time(NULL) - startTime;
        if (timeSoFar == 0)
            return 0;

        double speed = (double)doneSize / timeSoFar;
        if (speed == 0)
            return 0;

        return (int)((totalSize - doneSize) / speed);
    }

    void drawBar()
    {
        if (totalSize <= 0)
            return;

        int percent = (int)((double)doneSize / totalSize * 100);
        int timeLeft = calculateTimeLeft();

        cout << "\r" << setw(70) << " " << "\r";
        cout << taskName << ": " << percent << "% done";

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

    void update(long done)
    {
        doneSize = done;

        static time_t lastUpdate = 0;
        static long lastDone = 0;

        time_t now = time(NULL);
        bool timeForUpdate = (now - lastUpdate) >= 1;
        bool goodProgress = (doneSize - lastDone) > (totalSize / 100);

        if (timeForUpdate || goodProgress)
        {
            drawBar();
            lastUpdate = now;
            lastDone = doneSize;
        }
    }

    void finish()
    {
        update(totalSize);
        cout << endl
             << taskName << " finished in ";

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

struct HuffmanTree
{
    HuffmanTreeNode *root;
    string originalFileName;

    HuffmanTree()
    {
        root = NULL;
    }

    HuffmanTree(FrequencyCounter *counter)
    {
        PriorityQueue queue;

        for (int i = 0; i < 256; i++)
        {
            unsigned char byte = (unsigned char)i;
            if (counter->hasByte(byte))
            {
                HuffmanTreeNode *node = new HuffmanTreeNode(byte, counter->getCount(byte));
                queue.add(node);
            }
        }

        if (queue.size() == 1)
        {
            HuffmanTreeNode *singleNode = queue.getNext();
            root = new HuffmanTreeNode(0, singleNode->count);
            root->left = singleNode;
            return;
        }

        while (queue.size() > 1)
        {
            HuffmanTreeNode *left = queue.getNext();
            HuffmanTreeNode *right = queue.getNext();

            unsigned int totalCount = left->count + right->count;
            HuffmanTreeNode *parent = new HuffmanTreeNode(0, totalCount);
            parent->left = left;
            parent->right = right;

            queue.add(parent);
        }

        root = queue.getNext();
    }

    ~HuffmanTree()
    {
        delete root;
    }

    void buildCodeBook(HuffmanTreeNode *node, BitPattern currentCode, CodeBook *book)
    {
        if (node == NULL)
            return;

        if (node->isLeaf())
        {
            book->setCodeFor(node->symbol, currentCode);
            return;
        }

        BitPattern leftCode = currentCode;
        leftCode.bits[leftCode.length++] = false;
        buildCodeBook(node->left, leftCode, book);

        BitPattern rightCode = currentCode;
        rightCode.bits[rightCode.length++] = true;
        buildCodeBook(node->right, rightCode, book);
    }

    void saveNode(HuffmanTreeNode *node, ofstream &outFile)
    {
        if (node == NULL)
            return;

        if (node->isLeaf())
        {
            outFile.put(0);
            outFile.put(node->symbol);
        }
        else
        {
            outFile.put(1);
            saveNode(node->left, outFile);
            saveNode(node->right, outFile);
        }
    }

    void saveToFile(ofstream &outFile, const string &originalFile)
    {
        // Save original filename length and the name itself
        uint16_t nameLength = originalFile.length();
        outFile.write(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));
        outFile.write(originalFile.c_str(), nameLength);

        saveNode(root, outFile);
    }

    HuffmanTreeNode *loadNode(ifstream &inFile)
    {
        if (inFile.eof())
            return NULL;

        char nodeType;
        inFile.get(nodeType);

        if (nodeType == 0)
        {
            unsigned char byte;
            inFile.get(reinterpret_cast<char &>(byte));
            return new HuffmanTreeNode(byte, 0);
        }
        else
        {
            HuffmanTreeNode *node = new HuffmanTreeNode(0, 0);
            node->left = loadNode(inFile);
            node->right = loadNode(inFile);
            return node;
        }
    }

    bool loadFromFile(ifstream &inFile)
    {
        uint16_t nameLength;
        inFile.read(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));

        char *nameBuffer = new char[nameLength + 1];
        inFile.read(nameBuffer, nameLength);
        nameBuffer[nameLength] = '\0';
        originalFileName = nameBuffer;
        delete[] nameBuffer;

        root = loadNode(inFile);
        return root != NULL;
    }

    CodeBook *generateCodes()
    {
        CodeBook *book = new CodeBook();
        BitPattern currentCode;
        currentCode.length = 0;

        buildCodeBook(root, currentCode, book);
        return book;
    }

    HuffmanTreeNode *getRoot()
    {
        return root;
    }

    string getOriginalFileName()
    {
        return originalFileName;
    }
};

struct HuffmanCompressor
{
    int chunkSize;
    ProgressBar progress;

    HuffmanCompressor(int bufferSize)
    {
        chunkSize = bufferSize;
    }

    FrequencyCounter *countByteFrequencies(string filename)
    {
        ifstream file(filename, ios::binary);
        if (!file.is_open())
        {
            cerr << "Can't open file: " << filename << endl;
            return NULL;
        }

        FrequencyCounter *counter = new FrequencyCounter();

        file.seekg(0, ios::end);
        long totalSize = file.tellg();
        file.seekg(0, ios::beg);

        progress.setTotalSize(totalSize);
        progress.setTaskName("Analyzing file");
        progress.start();

        char *chunk = new char[chunkSize];
        long bytesProcessed = 0;

        while (!file.eof())
        {
            file.read(chunk, chunkSize);
            streamsize bytesRead = file.gcount();
            if (bytesRead <= 0)
                break;

            for (int i = 0; i < bytesRead; i++)
            {
                unsigned char byte = chunk[i];
                counter->addByte(byte);
            }

            bytesProcessed += bytesRead;
            progress.update(bytesProcessed);
        }

        delete[] chunk;
        file.close();
        progress.finish();

        return counter;
    }

    bool compressFileContents(string sourceFile, string destFile,
                              HuffmanTree *tree, CodeBook *codes)
    {
        ifstream inFile(sourceFile, ios::binary);
        if (!inFile.is_open())
        {
            cerr << "Can't open source file: " << sourceFile << endl;
            return false;
        }

        ofstream outFile(destFile, ios::binary);
        if (!outFile.is_open())
        {
            cerr << "Can't create output file: " << destFile << endl;
            inFile.close();
            return false;
        }

        inFile.seekg(0, ios::end);
        long totalSize = inFile.tellg();
        inFile.seekg(0, ios::beg);

        progress.setTotalSize(totalSize);
        progress.setTaskName("Compressing");
        progress.start();

        outFile << "ECE2103HUFFMAN" << endl;
        tree->saveToFile(outFile, sourceFile); // Save original filename too

        char *chunk = new char[chunkSize];
        unsigned char currentByte = 0;
        int bitsFilled = 0;
        long bytesProcessed = 0;

        while (!inFile.eof())
        {
            inFile.read(chunk, chunkSize);
            streamsize bytesRead = inFile.gcount();
            if (bytesRead <= 0)
                break;

            for (int i = 0; i < bytesRead; i++)
            {
                unsigned char byte = chunk[i];
                BitPattern code = codes->getCodeFor(byte);

                for (int j = 0; j < code.length; j++)
                {
                    if (code.bits[j])
                    {
                        currentByte |= (1 << (7 - bitsFilled));
                    }

                    bitsFilled++;

                    if (bitsFilled == 8)
                    {
                        outFile.put(currentByte);
                        currentByte = 0;
                        bitsFilled = 0;
                    }
                }
            }

            bytesProcessed += bytesRead;
            progress.update(bytesProcessed);
        }

        if (bitsFilled > 0)
        {
            outFile.put(currentByte);
        }

        delete[] chunk;
        inFile.close();
        outFile.close();
        progress.finish();

        return true;
    }

    bool extractFileContents(string sourceFile, string destFile)
    {
        ifstream inFile(sourceFile, ios::binary);
        if (!inFile.is_open())
        {
            cerr << "Can't open compressed file: " << sourceFile << endl;
            return false;
        }

        // First read file to get original filename if no destination specified
        string actualDestFile = destFile;
        if (destFile.empty())
        {
            char header[16];
            inFile.getline(header, 16);

            if (strcmp(header, "ECE2103HUFFMAN") != 0)
            {
                cerr << "This doesn't look like one of our compressed files" << endl;
                inFile.close();
                return false;
            }

            uint16_t nameLength;
            inFile.read(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));

            char *nameBuffer = new char[nameLength + 1];
            inFile.read(nameBuffer, nameLength);
            nameBuffer[nameLength] = '\0';
            actualDestFile = nameBuffer;
            delete[] nameBuffer;

            // Reset file position
            inFile.seekg(0, ios::beg);
        }

        ofstream outFile(actualDestFile, ios::binary);
        if (!outFile.is_open())
        {
            cerr << "Can't create output file: " << actualDestFile << endl;
            inFile.close();
            return false;
        }

        inFile.seekg(0, ios::end);
        long totalSize = inFile.tellg();
        inFile.seekg(0, ios::beg);

        progress.setTotalSize(totalSize);
        progress.setTaskName("Extracting");
        progress.start();

        char header[16];
        inFile.getline(header, 16);
        if (strcmp(header, "ECE2103HUFFMAN") != 0)
        {
            cerr << "This doesn't look like our compressed files" << endl;
            return false;
        }

        HuffmanTree *tree = new HuffmanTree();
        if (!tree->loadFromFile(inFile))
        {
            cerr << "Couldn't read data, dead file." << endl;
            delete tree;
            return false;
        }

        // If no destination specified, use original filename
        if (destFile.empty())
        {
            string originalName = tree->getOriginalFileName();
            cout << "Restoring to original filename: " << originalName << endl;
        }

        HuffmanTreeNode *currentNode = tree->getRoot();
        char *chunk = new char[chunkSize];
        long bytesProcessed = 0;

        while (!inFile.eof())
        {
            inFile.read(chunk, chunkSize);
            streamsize bytesRead = inFile.gcount();
            if (bytesRead <= 0)
                break;

            for (int i = 0; i < bytesRead; i++)
            {
                unsigned char byte = chunk[i];

                for (int bit = 7; bit >= 0; bit--)
                {
                    bool isOne = (byte & (1 << bit)) != 0;

                    if (isOne)
                    {
                        currentNode = currentNode->right;
                    }
                    else
                    {
                        currentNode = currentNode->left;
                    }

                    if (currentNode->isLeaf())
                    {
                        outFile.put(currentNode->symbol);
                        currentNode = tree->getRoot();
                    }
                }
            }

            bytesProcessed += bytesRead;
            progress.update(bytesProcessed);
        }

        delete[] chunk;
        delete tree;
        inFile.close();
        outFile.close();
        progress.finish();

        return true;
    }

    bool zipFile(string sourceFile, string destFile)
    {
        // If destination not specified, use source filename + extension
        if (destFile.empty())
        {
            destFile = sourceFile + ".ece2103";
        }

        FrequencyCounter *counter = countByteFrequencies(sourceFile);
        if (!counter)
            return false;

        cout << "Building compression tree..." << endl;
        HuffmanTree *huffTree = new HuffmanTree(counter);

        cout << "Creating compression codes..." << endl;
        CodeBook *codes = huffTree->generateCodes();

        bool success = compressFileContents(sourceFile, destFile, huffTree, codes);

        delete counter;
        delete huffTree;
        delete codes;

        return success;
    }

    bool unzipFile(string sourceFile, string destFile)
    {
        return extractFileContents(sourceFile, destFile);
    }
};

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
    cout << "  - Default = 4KB " << endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        showHelp();
        return 1;
    }

    bool wantToCompress = false;
    bool wantToDecompress = false;
    int chunkSize = 4096;
    string sourceFile = "";
    string destFile = "";

    for (int i = 1; i < argc; i++)
    {
        string option = argv[i];

        if (option == "-c")
        {
            wantToCompress = true;

            // Require at least source file
            if (i + 1 < argc)
            {
                sourceFile = argv[i + 1];
                i++;

                // Check if destination file is also provided
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    destFile = argv[i + 1];
                    i++;
                }
            }
            else
            {
                cout << "Error: Missing source file after -c" << endl;
                showHelp();
                return 1;
            }
        }
        else if (option == "-d")
        {
            wantToDecompress = true;

            // Require at least source file
            if (i + 1 < argc)
            {
                sourceFile = argv[i + 1];
                i++;

                // Check if destination file is also provided
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    destFile = argv[i + 1];
                    i++;
                }
            }
            else
            {
                cout << "Error: Missing source file after -d" << endl;
                showHelp();
                return 1;
            }
        }
        else if (option == "-b" && i + 1 < argc)
        {
            chunkSize = stoi(argv[i + 1]);
            i++;
        }
        else
        {
            cout << "Don't understand: " << option << endl;
            showHelp();
            return 1;
        }
    }

    HuffmanCompressor zipTool(chunkSize);

    clock_t startTime = clock();
    bool allGood = false;

    if (wantToCompress)
    {
        cout << "Compressing " << sourceFile;
        if (!destFile.empty())
        {
            cout << " to " << destFile;
        }
        else
        {
            cout << " to " << sourceFile << ".ece2103";
        }
        cout << endl;

        cout << "Using chunk size: " << chunkSize << " bytes" << endl;
        allGood = zipTool.zipFile(sourceFile, destFile);
    }
    else if (wantToDecompress)
    {
        cout << "Extracting " << sourceFile;
        if (!destFile.empty())
        {
            cout << " to " << destFile;
        }
        cout << endl;

        cout << "Using chunk size: " << chunkSize << " bytes" << endl;
        allGood = zipTool.unzipFile(sourceFile, destFile);
    }
    else
    {
        cout << "\nPlease tell me what to do. Use -c to compress or -d to decompress.\n"
             << endl;
        showHelp();
        return 1;
    }

    clock_t endTime = clock();
    double seconds = double(endTime - startTime) / CLOCKS_PER_SEC;

    if (allGood)
    {
        cout << "\nThat took " << seconds << " seconds." << endl;
        return 0;
    }
    else
    {
        cout << "\nError." << endl;
        return 1;
    }
}