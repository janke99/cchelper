#pragma once

#include "IGameWindow.h"
#include "IChessEngine.h"

#define INPUT_BUFFER_SIZE		(4096)
struct PipeStruct;

class CChessEngine
	:public IChessEngine
{
public:
	struct PieceMove
	{
		int fx;
		int fy;
		int tx;
		int ty;
	};

	enum EngineState
	{
		BusyWait,
		Idle,
	};

private:
	PipeStruct * m_pPipe;
	bool m_bLoaded;
	char m_szInputBuf[INPUT_BUFFER_SIZE];

	EngineState m_state;

	bool m_bHasBestMove;
	PieceMove m_mvBestMove;
	int m_nSkipBestMoveCount;

	IGameWindow * m_pGameWindow;

public:
	CChessEngine(void);
	virtual ~CChessEngine(void);

	void SetGameWindow(IGameWindow * pgw)
	{
		m_pGameWindow = pgw;
	}

	EngineState GetState() { return m_state; }

	PieceMove * GetBestMove() { if ( m_bHasBestMove ) return &m_mvBestMove ; else return NULL ; };

	void UpdateState();
	void Stop();
	
	void SendCommand(const char * cmd);

	bool IsLoaded(){ return m_bLoaded;}
	bool InitEngine(TCHAR * szEngineFile);

	virtual void Restart();
	virtual void GameOver();
};
