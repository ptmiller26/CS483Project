#ifndef _EVOAUDIO_H_
#define _EVOAUDIO_H_

#define MONO	(0)
#define STEREO	(1)

class EvoAudio
{
   public:
	EvoAudio();
	~EvoAudio();

	// Init sound system
	void Initialize();

	// Update sound system
	void Update(float fDeltaT);

	// Load all the sounds
	bool LoadSoundBank(char *szFilename);

	// Play Sound
	bool TriggerSound(char *szSound);

	void UpdateListener(EvoMatrix *pM);

   protected:
	IXACTEngine		*m_pXACT;

	IXACTWaveBank	*m_pWaveBank;
	IXACTSoundBank	*m_pSoundBank;

	uchar			*m_pWaveBankBytes;
	uchar			*m_pSoundBankBytes;

	// 3D
	X3DAUDIO_LISTENER	m_Listener;
	X3DAUDIO_CONE		m_Cone;

	// need to instance
	X3DAUDIO_DSP_SETTINGS m_aDSPSettings[2];


};


#endif
