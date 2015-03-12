#ifndef _EVOPARSER_H_
#define _EVOPARSER_H_

// Maximum String Length for GroupNames, KeyNames, and KeyValues
#define MAX_PARSER_STRING_LENGTH	512

// Each Key has a Name and a Value
struct KEYINFO {
	char KeyName[MAX_PARSER_STRING_LENGTH];
	char KeyValue[MAX_PARSER_STRING_LENGTH];
	int KeyValueInteger;
	float KeyValueFloat;
};

// Each Group has a Name, and an array of Keys
struct GROUPINFO {
	char GroupName[MAX_PARSER_STRING_LENGTH];
	int NumKeys;
	KEYINFO *Keys;
};

class EvoParser {
   private:
	int NumGroupKeys;
	GROUPINFO *GroupKeys;
	GROUPINFO *SelectedGroup;

   public:
	EvoParser();			
	EvoParser(char *lpFilename);
	
	~EvoParser();
	
	bool Initialize(char *lpFilename);

	// Sets the Group Key for all Subsequent Gets
	// Returns TRUE if Group Key Found, FALSE otherwise
	bool SetGroupKey(char *GroupKey);

	// Put the Value of the Key into VALUE(as either int or string)
	// VALUE will be NULL if the Key was not found and will return FALSE
	bool GetKeyValue(char *Key, int *Value);
	bool GetKeyValue(char *Key, uint *Value);
	bool GetKeyValue(char *Key, float *Value);
	bool GetKeyValue(char *Key, char *Value);

	bool SetKeyValue(char *Key, int *Value);
	bool SetKeyValue(char *Key, uint *Value);
	bool SetKeyValue(char *Key, float *Value);
	bool SetKeyValue(char *Key, char *Value);
	


	// Gets all the Keys and Values for the specified Group, if GroupKey is NULL
	// will get the Group Set with SETGROUPKEY function.  Count will be filled with
	// the Number of Keys written into Result
	bool GetWholeGroup(char *GroupKey, int &Count, KEYINFO *&Keys); 

	// Write Parser Back Out to a File
	bool WriteParser(char *Filename, bool bBinary);

};

void Encode(char *lpString, bool bEncode);


#endif