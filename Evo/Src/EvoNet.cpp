#include "EvoStdAfx.h"

unsigned char GetPacketIdentifier(Packet *p);


EvoNet::EvoNet()
{
	m_pRakPeer = NULL;
}

EvoNet::~EvoNet()
{
	if (m_pRakPeer)
	{
		m_pRakPeer->Shutdown(300);
		RakNetworkFactory::DestroyRakPeerInterface(m_pRakPeer);
		m_pRakPeer = NULL;

	}
}

void EvoNet::Initialize(uint uiServerPort, uint uiClientPort)
{
	m_uiServerPort = uiServerPort;
	m_uiClientPort = uiClientPort;

	m_pRakPeer = RakNetworkFactory::GetRakPeerInterface();

	m_bHost = false;
}

bool EvoNet::Host()
{
	m_bHost = true;
	bool bResult = m_pRakPeer->Startup(EVONET_MAXCONNECTIONS, 30, &SocketDescriptor(m_uiServerPort, 0), 1);
	m_pRakPeer->SetMaximumIncomingConnections(EVONET_MAXCONNECTIONS); 

	return bResult;
}

bool EvoNet::Join(char *szIP)
{
	m_bHost = false;

	bool bResult;
	bResult = m_pRakPeer->Startup(1, 30, &SocketDescriptor(), 1);
	bResult = m_pRakPeer->Connect(szIP, m_uiServerPort, 0, 0);

	return bResult;
}

void EvoNet::Update(float fDeltaT)
{
	Packet *pPacket = m_pRakPeer->Receive();

	while (pPacket != NULL)
	{
		HandlePacket(pPacket);
		m_pRakPeer->DeallocatePacket(pPacket);

		pPacket = m_pRakPeer->Receive();
	} 

	
}

void EvoNet::SendData(void *pData, uint uiLength)
{
	m_pRakPeer->Send((const char *)pData, uiLength, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
}

void EvoNet::HandlePacket(Packet *pPacket)
{
	unsigned char ucID = GetPacketIdentifier(pPacket);

	switch(ucID)
	{
		case ID_NEW_INCOMING_CONNECTION:
			break;

		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			break;
	};
	
	char szMsg[256];
	sprintf(szMsg, "EvoNet: %d\n", (uint)GetPacketIdentifier(pPacket));
	OutputDebugString(szMsg);
}

// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
		return (unsigned char) p->data[0];
}