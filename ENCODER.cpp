#include <bits/stdc++.h>
using namespace std;
#include <fstream>

struct Node
{
    unsigned char data;
    long long int freq;
    Node *left, *right;

    Node()
    {
        left = right = NULL;
    }
};

class huffman
{
private:
    vector<Node *> arr;
    vector<long long int> Huffman_codemap;

    string inFileName;

    Node *root;

    class Compare
    {
    public:
        bool operator()(Node *l, Node *r)
        {
            return l->freq > r->freq;
        }
    };

    priority_queue<Node *, vector<Node *>, Compare> minHeap;

    void createArr()
    {
        for (int i = 0; i < 256; i++)
        {
            arr.push_back(new Node());
            arr[i]->data = i;
            arr[i]->freq = 0;
        }
    }

    void createMinHeap()
    {
        char id;
        fstream input_file;
        input_file.open(inFileName, ios::in);
        input_file.get(id);
        // Incrementing frequency of datas that appear in the input file
        while (!input_file.eof())
        {
            arr[id]->freq++;
            input_file.get(id);
        }
        input_file.close();
        // Pushing the Nodes which appear in the file into the priority queue (Min Heap)
        for (int i = 0; i < 256; i++)
        {
            if (arr[i]->freq > 0)
            {
                minHeap.push(arr[i]);
            }
        }
    }

    void createTree()
    {
        // Creating Huffman Tree with the Min Heap created earlier
        Node *left, *right;
        priority_queue<Node *, vector<Node *>, Compare> tempPQ(minHeap);
        while (tempPQ.size() != 1)
        {
            left = tempPQ.top();
            tempPQ.pop();

            right = tempPQ.top();
            tempPQ.pop();

            root = new Node();
            root->freq = left->freq + right->freq;

            root->left = left;
            root->right = right;
            tempPQ.push(root);
        }
    }

    void store_codes(Node *Root, vector<char> single_code, int index)
    {
        if (Root->left)
        {
            single_code[index] = '0';
            store_codes(Root->left, single_code, index + 1);
        }
        if (Root->right)
        {
            single_code[index] = '1';
            store_codes(Root->right, single_code, index + 1);
        }
        if (!Root->left && !Root->left)
        {
            for (int i = index; i >= 0; i--)
            {
                if (i != index)
                {
                    Huffman_codemap[Root->data] *= 10;
                    Huffman_codemap[Root->data] += single_code[i] - '0';
                }
                else
                    Huffman_codemap[Root->data] = 1;
            }
        }
    }

    void store_tree(ofstream &input, Node *Root) // Write tree to file
    {

        if (!Root->left && !Root->right)
        {
            input << '1';
            input << Root->data;
        }
        else
        {
            input << '0';
            store_tree(input, Root->left);
            store_tree(input, Root->right);
        }
    }

    void Write_compressed(ifstream &input, ofstream &output) // Write to a new file(Compressed)
    {
        char ch;
        unsigned char bits_8;
        long long int counter = 0;
        while (input.get(ch))
        {
            long long int temp = Huffman_codemap[static_cast<unsigned char>(ch)];
            while (temp != 1)
            {
                bits_8 <<= 1;
                if ((temp % 10) != 0)
                    bits_8 |= 1;
                temp /= 10;
                counter++;
                if (counter == 8)
                {
                    output << bits_8;
                    counter = bits_8 = 0;
                }
            }
        }
        while (counter != 8)
        {
            bits_8 <<= 1;
            counter++;
        }
        output << bits_8;
        output.close();
    }

public:
    // Constructor
    huffman(string inFileName)
    {
        this->inFileName = inFileName;
        createArr();
    }
    void compress()
    {
        createMinHeap();
        createTree();

        Huffman_codemap.resize(256);
        ifstream input_file(inFileName.c_str(), ios::binary);
        if (!input_file.good())
        {
            perror("Error:\t");
            exit(-1);
        }

        ofstream output_file((inFileName + ".huf").c_str(), ios::binary);
        if (!output_file.good())
        {
            perror("Error:\t");
            exit(-1);
        }

        output_file << root->freq;
        output_file << ',';
        store_tree(output_file, root);
        output_file << ' ';

        vector<char> single_code(16);
        store_codes(root, single_code, 0);

        Write_compressed(input_file,output_file);                                    
        input_file.close();                                                                  
        output_file.close(); 
    }
};

int main()
{
    string input;
    cout << "Enter file name you want to compress: ";
    cin >> input;

    cout << "\nCompressing the file.....";
    clock_t start_time = clock();

    huffman f(input);
    f.compress();

    clock_t stop_time = clock();
    cout << "\n\n*********************************************File Compressed Successfully! :-)*********************************************\n\n";
    cout << "Time taken to Compress:\t" << double(stop_time - start_time) / CLOCKS_PER_SEC << " seconds\n\n";

    return 0;
}