#ifndef __QTMULTIPLAYER__
#define __QTMULTIPLAYER__

#include <vector>
#include "QtPlayer.hxx"

namespace CLAM
{
    namespace VM
    {
	class Player;
		
	class QtMultiPlayer : public QtPlayer
	{
	    Q_OBJECT

	public:
	    QtMultiPlayer(QWidget* parent=0);
	    ~QtMultiPlayer();

	    void SetPlaySegment(const MediaTime& time);
	    bool IsPlaying();
				
        public slots:
	    virtual void play();
	    virtual void pause();
	    virtual void stop();

	protected:
	    std::vector<Player*> mPlayers;
	    int mCurrentPlayer;
	    		
	    void AddPlayer(Player* player);

	    virtual void SetCurrentPlayer(int playerID)=0;
							
	};
    }
}

#endif
