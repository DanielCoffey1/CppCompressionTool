#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <functional>

// Define a structure for a Huffman tree node
struct HuffmanNode {
	char data;
	unsigned frequency;
	HuffmanNode* left;
	HuffmanNode* right;

	HuffmanNode(char data, unsigned frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
};

// Comparison function for priority queue
struct Compare {
	bool operator()(HuffmanNode* a, HuffmanNode* b) {
		return a->frequency > b->frequency;
	}
};

// Helper function to deallocate the memory used by the Huffman tree
void deallocateTree(HuffmanNode* node) {
	if (node == nullptr) {
		return;
	}

	deallocateTree(node->left);
	deallocateTree(node->right);

	delete node;
}

// Function to generate the Huffman tree
HuffmanNode* buildHuffmanTree(const std::string& text) {
	// Calculate the frequency of each character
	std::unordered_map<char, unsigned> frequencyMap;
	for (char c : text) {
		frequencyMap[c]++;
	}

	// Create a priority queue to store the nodes
	std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;

	// Create a leaf node for each character and add it to the priority queue
	for (const auto& pair : frequencyMap) {
		pq.push(new HuffmanNode(pair.first, pair.second));
	}

	// Build the Huffman tree by merging the nodes
	while (pq.size() > 1) {
		HuffmanNode* left = pq.top();
		pq.pop();
		HuffmanNode* right = pq.top();
		pq.pop();

		HuffmanNode* newNode = new HuffmanNode('\0', left->frequency + right->frequency);
		newNode->left = left;
		newNode->right = right;

		pq.push(newNode);
	}

	return pq.top();
}

// Function to write a single bit to the output file
void writeBit(std::ofstream& outputFile, bool bit) {
	static unsigned char currentByte = 0;
	static unsigned char bitCount = 0;

	currentByte |= (bit << (7 - bitCount));

	if (++bitCount == 8) {
		outputFile.write(reinterpret_cast<const char*>(&currentByte), sizeof(currentByte));
		currentByte = 0;
		bitCount = 0;
	}
}

// Function to write the Huffman tree to the output file
void writeTree(std::ofstream& outputFile, HuffmanNode* node) {
	if (node == nullptr) {
		return;
	}

	if (node->data == '\0') {
		writeBit(outputFile, false);
		writeTree(outputFile, node->left);
		writeTree(outputFile, node->right);
	}
	else {
		writeBit(outputFile, true);
		outputFile.write(&node->data, sizeof(node->data));
	}
}

// Function to encode the input file using the Huffman tree
void encodeFile(const std::string& inputFileName, const std::string& outputFileName) {
	std::ifstream inputFile(inputFileName, std::ios::binary);
	std::ofstream outputFile(outputFileName, std::ios::binary);

	if (!inputFile || !outputFile) {
		std::cerr << "Failed to open files." << std::endl;
		return;
	}

	// Read the contents of the input file into a string
	std::string text((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

	//
