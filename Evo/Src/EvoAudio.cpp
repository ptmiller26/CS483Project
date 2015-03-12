#include "EvoStdAfx.h"

EvoAudio::EvoAudio()
{

}

EvoAudio::~EvoAudio()
{

}

void EvoAudio::Initialize()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CoCreateInstance(__uuidof(XACTEngine), NULL, CLSCTX_INPROC_SERVER, __uuidof(IXACTEngine), (void**)&m_pXACT);

	XACT_RUNTIME_PARAMETERS Params;

	memset(&Params, 0, sizeof(Params));

	m_pXACT->Initialize(&Params);

	// 3D Setup
	
	// Mono sounds
	m_aDSPSettings[MONO].SrcChannelCount = 1;							
	m_aDSPSettings[MONO].DstChannelCount = 2;    // must match number of channels of SpeakerChannelMask sent to XACT3DCalculate
	m_aDSPSettings[MONO].pMatrixCoefficients = new FLOAT32[m_aDSPSettings[MONO].SrcChannelCount * m_aDSPSettings[MONO].DstChannelCount];

	// Stereo sounds
	m_aDSPSettings[STEREO].SrcChannelCount = 2;							
	m_aDSPSettings[STEREO].DstChannelCount = 2;    // must match number of channels of SpeakerChannelMask sent to XACT3DCalculate
	m_aDSPSettings[STEREO].pMatrixCoefficients = new FLOAT32[m_aDSPSettings[STEREO].SrcChannelCount * m_aDSPSettings[STEREO].DstChannelCount];


}

void EvoAudio::UpdateListener(EvoMatrix *pM)
{
	pM->GetTranslate((EvoPoint3 *)&m_Listener.Position);
	pM->GetRow(2, (EvoPoint3 *)&m_Listener.OrientFront);
	pM->GetRow(1, (EvoPoint3 *)&m_Listener.OrientTop);

	m_Listener.Velocity.x = m_Listener.Velocity.y = m_Listener.Velocity.z = 0.0f;
	m_Listener.pCone = NULL;
}


void EvoAudio::Update(float fDeltaT)
{
	m_pXACT->DoWork();
}

// Load all the sounds
bool EvoAudio::LoadSoundBank(char *szFilename)
{
	char szString[256];
	sprintf(szString, "%s%s", szFilename, ".xwb");
	EvoFile *pFile = EvoFile::OpenFile(szString, EvoFile::READ);
	uint uiFileSize = pFile->GetSize();

	m_pWaveBankBytes = new uchar[uiFileSize];
	pFile->ReadBytes(m_pWaveBankBytes, uiFileSize);

	m_pXACT->CreateInMemoryWaveBank( m_pWaveBankBytes, uiFileSize, 0 , 0, &m_pWaveBank );
	EvoFile::CloseFile(pFile);

	sprintf(szString, "%s%s", szFilename, ".xsb");
	pFile = EvoFile::OpenFile(szString, EvoFile::READ);
	uiFileSize = pFile->GetSize();

	m_pSoundBankBytes = new uchar[uiFileSize];
	pFile->ReadBytes(m_pSoundBankBytes, uiFileSize);

	m_pXACT->CreateSoundBank( m_pSoundBankBytes, uiFileSize, 0 , 0, &m_pSoundBank );
	EvoFile::CloseFile(pFile);
	return true;
}

// Play Sound
bool EvoAudio::TriggerSound(char *szSound)
{
	XACTINDEX XCueIndex_gs1;

	// Look up the cue index from the friendly name
	XCueIndex_gs1 = m_pSoundBank->GetCueIndex(szSound);
	// Trigger the cue
	m_pSoundBank->Play(XCueIndex_gs1, 0, 0, NULL);

	return true;
}
