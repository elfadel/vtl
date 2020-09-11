#ifndef _MP4STREAMER_H_
#define _MP4STREAMER_H_

#include <list>
#include <iostream>

using namespace std;
void timeoutHandler(int);

class PlayList {
public:
	PlayList();
	const char* getNextVideo(void);
	void addVideoToPlayList(const char* video);
	int gotoTopPlayList(void);
	void printPlayList(void);
private:
	static std::list<const char*>* playlist;
	static std::list<const char*>::iterator prevVideo;
};

#endif //_MP4STREAMER_H_

list <const char*>* PlayList::playlist = new list<const char*>;
list <const char*>::iterator PlayList::prevVideo = PlayList::playlist->begin();

PlayList::PlayList() {
	playlist->clear();
}

void PlayList::addVideoToPlayList(const char* video) {
	playlist->push_back(video);
}

int PlayList::gotoTopPlayList(void) {
	if(playlist->empty())
		return -1;
	else {
		prevVideo = playlist->begin();
		return 0;
	}
}

const char* PlayList::getNextVideo(void) {
	list<const char*>::iterator nextVideo = prevVideo++;
	if(prevVideo == playlist->end())
		prevVideo = playlist->begin();
	return(*(nextVideo));
}

void PlayList::printPlayList(void) {
	int j = 0;
	for(list<const char*>::iterator i = playlist->begin(); i != playlist->end(); ++i)
		std::cout << ++j << ". " << *i << std::endl;
}