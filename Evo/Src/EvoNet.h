#ifndef _EVONET_H_
#define _EVONET_H_

#define EVONET_MAXCONNECTIONS	(8)

class EvoNet
{
   public:
	EvoNet();
	~EvoNet();

	// Init 
	void Initialize(uint uiServerPort, uint uiClientPort);

	bool Host();

	bool Join(char *szIP); 

	void Update(float fDeltaT);

	void SendData(void *pData, uint uiLength);

   protected:
	void HandlePacket(Packet *pPacket);

   protected:
	RakPeerInterface* m_pRakPeer;

	uint m_uiServerPort;
	uint m_uiClientPort;

	bool m_bHost;
};


#endif