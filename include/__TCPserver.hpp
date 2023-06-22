#include <fstream>

class FileHandler {
	std::ifstream fileStream;
	std::string pathToFile;
	std::string buffer;
	unsigned int fileSize;
	bool isOpened = false;

public:
	FileHandler(std::string pathToFile) {
		this->pathToFile = pathToFile;
		this->fileStream.open(pathToFile);
		this->isOpened = true;
	}
	bool isOpen(void) {
		return this->fileStream.is_open();
	}
	std::string getLine() {
		buffer.clear();
		if (this->isOpen())
			std::getline(fileStream, buffer);
		return buffer;
	}
	void seek(unsigned int value) {
		this->fileStream.seekg(value);
	}

	std::ifstream getFileStream(void) {
		
	}
	void close() {
		this->fileStream.close();
	}
	~FileHandler() {
		//this->close();
	}
};