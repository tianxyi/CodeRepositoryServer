#include "IPublisher.h"

class Publisher : public IPublisher {
public:
	virtual void setCmd() override {
		int argc_ = 7;
		char argv_[7][30] = { "D:\test\AmmarProject2Sol\Debug" ,"..","/s","/demo","*.h","*.cpp","[A-B](.*)" };


	}
	virtual void convert() override {


	}
	virtual std::vector<std::string> getConvertedFilesList() override {
		return files_;
	}
	~Publisher() {};
private:
	std::vector<std::string> files_;
};

IPublisher * PublisherFactory::createPublisher()
{
	return new Publisher;
}
