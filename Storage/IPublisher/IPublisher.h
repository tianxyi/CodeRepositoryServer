#pragma once
#include <vector>
#include <string>
class IPublisher {
public:
	virtual void setCmd() = 0;
	virtual void convert() = 0;
	virtual std::vector<std::string> getConvertedFilesList() = 0;
	virtual ~IPublisher() {};
};

extern "C" {
	struct PublisherFactory {
		//DLL_DECL
		IPublisher* createPublisher();
		// *IPublisher createPublisher(){
	//		return new Publisher;
	//}
	};
}