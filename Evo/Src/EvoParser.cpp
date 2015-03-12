#include "EvoStdAfx.h"


void GetToken(FILE *ifp, char *Token, bool bWholeLine, bool bDecode);

EvoParser::EvoParser()
{
	NumGroupKeys = 0;
	GroupKeys = NULL;
}

EvoParser::EvoParser(char *lpFilename)
{
	Initialize(lpFilename);
}

// Destructor
EvoParser::~EvoParser()
{
	int cnt;

	for (cnt = 0; cnt < NumGroupKeys; cnt++)
	{
		SAFE_DELETE(GroupKeys[cnt].Keys);
	}

	SAFE_DELETE(GroupKeys);

}

bool EvoParser::Initialize(char *lpFilename)
{
	bool bBinFlag = false;
	FILE *ifp = NULL;
	int cnt;
	char Token[MAX_PARSER_STRING_LENGTH];
	int KeysInGroup[1000];
	int Counter = 0;

	NumGroupKeys = 0;
	SelectedGroup = NULL;

	char BinFilename[512];
	strcpy(BinFilename, lpFilename);
	int Len = strlen(BinFilename);
	strcpy(&BinFilename[Len-3], "pse");

	if ((ifp = fopen(BinFilename, "rb")) == NULL) {
		if ((ifp = fopen(lpFilename, "r")) == NULL)
			return false;
		bBinFlag = true;
	}

	bool bDecode = !bBinFlag;

	GetToken(ifp, Token, false, bDecode);
	// First Determine How Many Groups There are in the File
	while (strcmp(Token, "*EOF*")) {
		if (!strcmp(Token, "]")) {
			if (NumGroupKeys != 0) {
				KeysInGroup[NumGroupKeys-1] = Counter;
			}
			Counter = 0;
			NumGroupKeys++;
		}
		else if (!strcmp(Token, "=")) {
			Counter++;
		}
		GetToken(ifp, Token, false, bDecode);
	}

	if (NumGroupKeys != 0) {
		KeysInGroup[NumGroupKeys-1] = Counter;
	}

	GroupKeys = new GROUPINFO[NumGroupKeys];
	for (cnt = 0; cnt < NumGroupKeys; cnt++) {
		GroupKeys[cnt].NumKeys = KeysInGroup[cnt];
		GroupKeys[cnt].Keys = new KEYINFO[KeysInGroup[cnt]];
	}

	rewind(ifp);
	
	Counter = 0;
	NumGroupKeys = -1;

	GetToken(ifp, Token,false, bDecode);
	// Now Fill in Group and Key Info
	while (strcmp(Token, "*EOF*")) {
		if (!strcmp(Token, "[")) {
			NumGroupKeys++;
			GetToken(ifp,Token,false, bDecode);
			strcpy(GroupKeys[NumGroupKeys].GroupName, Token);
			Counter = 0;
		}
		else if (strcmp(Token, "]")) {
			strcpy(GroupKeys[NumGroupKeys].Keys[Counter].KeyName, Token);
			GetToken(ifp, Token,false, bDecode);
			if (strcmp(Token, "=")) {
				// Syntax Error
			}
			GetToken(ifp, Token, true, bDecode);
			strcpy(GroupKeys[NumGroupKeys].Keys[Counter].KeyValue, Token);
			GroupKeys[NumGroupKeys].Keys[Counter].KeyValueInteger = atoi(Token);
			GroupKeys[NumGroupKeys].Keys[Counter].KeyValueFloat = (float)atof(Token);
			Counter++;
		}
		GetToken(ifp, Token, false, bDecode);
	}
	NumGroupKeys++;

	fclose(ifp);

//	if (bBinFlag)
//		WriteParser(BinFilename, true);

	return true;
}

// Sets the Group Key for all Subsequent Gets
// Returns true if Group Key Found, false otherwise
bool EvoParser::SetGroupKey(char *GroupKey)
{
	int cnt;

	for (cnt = 0; cnt < NumGroupKeys; cnt++) {
		if (!strcmp(GroupKey, GroupKeys[cnt].GroupName)) {
			SelectedGroup = &GroupKeys[cnt];
			return true;
		}
	}
	SelectedGroup = NULL;
	return false;
}

// Put the Value of the Key into VALUE(as either int or string)
// VALUE will be NULL if the Key was not found and will return false
bool EvoParser::GetKeyValue(char *Key, int *Value)
{
	int cnt;

	if (SelectedGroup == NULL) {
		Value = NULL;
		return false;
	}
	
	for (cnt = 0; cnt < SelectedGroup->NumKeys; cnt++) {
		if (!strcmp(SelectedGroup->Keys[cnt].KeyName, Key)) {
			*Value = SelectedGroup->Keys[cnt].KeyValueInteger;
			return true;
		}
	}

	return false;
}

bool EvoParser::GetKeyValue(char *Key, uint *Value)
{
	return GetKeyValue(Key, (int *)Value);
}

bool EvoParser::GetKeyValue(char *Key, float *Value)
{
	int cnt;

	if (SelectedGroup == NULL) {
		*Value = 0.0F;
		return false;
	}
	
	for (cnt = 0; cnt < SelectedGroup->NumKeys; cnt++) {
		if (!strcmp(SelectedGroup->Keys[cnt].KeyName, Key)) {
			*Value = SelectedGroup->Keys[cnt].KeyValueFloat;
			return true;
		}
	}

	return false;

}

bool EvoParser::GetKeyValue(char *Key, char *Value)
{	
	int cnt;

	if (SelectedGroup == NULL) {
		Value = NULL;
		return false;
	}
	
	for (cnt = 0; cnt < SelectedGroup->NumKeys; cnt++) {
		if (!strcmp(SelectedGroup->Keys[cnt].KeyName, Key)) {
			strcpy(Value, SelectedGroup->Keys[cnt].KeyValue);
			return true;
		}
	}

	return false;

}
	



bool EvoParser::SetKeyValue(char *Key, int *Value)
{
	int cnt;

	if (SelectedGroup == NULL) {
		Value = NULL;
		return false;
	}
	
	for (cnt = 0; cnt < SelectedGroup->NumKeys; cnt++) {
		if (!strcmp(SelectedGroup->Keys[cnt].KeyName, Key)) {
			SelectedGroup->Keys[cnt].KeyValueInteger = *Value;
			SelectedGroup->Keys[cnt].KeyValueFloat = (float)(*Value);
			wsprintf(SelectedGroup->Keys[cnt].KeyValue, "%d", *Value);
			return true;
		}
	}

	return false;

}

bool EvoParser::SetKeyValue(char *Key, uint *Value)
{
	return SetKeyValue(Key, (int *)Value);
}

bool EvoParser::SetKeyValue(char *Key, float *Value)
{
	int cnt;

	if (SelectedGroup == NULL) {
		*Value = 0.0F;
		return false;
	}
	
	for (cnt = 0; cnt < SelectedGroup->NumKeys; cnt++) {
		if (!strcmp(SelectedGroup->Keys[cnt].KeyName, Key)) {
			SelectedGroup->Keys[cnt].KeyValueInteger = (int)(*Value);
			SelectedGroup->Keys[cnt].KeyValueFloat = *Value;
			wsprintf(SelectedGroup->Keys[cnt].KeyValue, "%f", *Value);
			return true;
		}
	}

	return false;

}

bool EvoParser::SetKeyValue(char *Key, char *Value)
{	
	int cnt;

	if (SelectedGroup == NULL) {
		Value = NULL;
		return false;
	}
	
	for (cnt = 0; cnt < SelectedGroup->NumKeys; cnt++) {
		if (!strcmp(SelectedGroup->Keys[cnt].KeyName, Key)) {
			strcpy(SelectedGroup->Keys[cnt].KeyValue, Value);
			SelectedGroup->Keys[cnt].KeyValueInteger = atoi(SelectedGroup->Keys[cnt].KeyValue);
			SelectedGroup->Keys[cnt].KeyValueFloat = (float)atof(SelectedGroup->Keys[cnt].KeyValue);
			return true;
		}
	}

	return false;

}


// Gets all the Keys and Values for the specified Group, if GroupKey is NULL
// will get the Group Set with SETGROUPKEY function.  Count will be filled with
// the Number of Keys written into Result
bool EvoParser::GetWholeGroup(char *GroupKey, int &Count, KEYINFO *&Keys)
{
	if (GroupKey == NULL) {
		Count = SelectedGroup->NumKeys;
		Keys = SelectedGroup->Keys;
		return true;
	}
	else if (SetGroupKey(GroupKey)) {
		Count = SelectedGroup->NumKeys;
		Keys = SelectedGroup->Keys;
		return true;
	}

	Keys = NULL;
	Count = 0;
	return false;

}


// Retrieves a Token from the File
void GetToken(FILE *ifp, char *Token, bool bWholeLine, bool bDecode)
{
	char c;
	char String[MAX_PARSER_STRING_LENGTH];
	fpos_t pos;

	Token[0] = NULL;

	if (feof(ifp)) {
		strcpy(Token, "*EOF*");
		return;
	}

	fread(&c, sizeof(char), 1, ifp);
	
	if (bDecode)
		c -= 1;

	if (feof(ifp)) {
		strcpy(Token, "*EOF*");
		return;
	}

	if (c == ';') { // Is a Comment
		while (c != '\n' && !feof(ifp)) {
			fread(&c, sizeof(char), 1, ifp);
			if (bDecode)
				c -= 1;
		}
		GetToken(ifp, Token, bWholeLine, bDecode);
	}
	else if (c == '\n' || c == ' ' || c == '\t') {
		GetToken(ifp, Token, bWholeLine, bDecode);
	}
	else if (c == '[') {
		strcpy(Token, "[");
	}
	else if (c == ']') {
		strcpy(Token, "]");
	}
	else if (c == '=') {
		strcpy(Token, "=");
	}
	else if (bWholeLine) {
		while (c != '\n' && !feof(ifp)) {
			fgetpos(ifp, &pos);
			sprintf(String, "%c", c);
			strcat(Token, String);
			fread(&c, sizeof(char), 1, ifp);
			if (bDecode)
				c -= 1;
			if (c == ']') 
				fsetpos(ifp, &pos);
		}
	}
	else {
		while (c != '\n' && c != ' ' && c != '\t' && c != ']' && !feof(ifp)) {
			fgetpos(ifp, &pos);
			sprintf(String, "%c", c);
			strcat(Token, String);
			fread(&c, sizeof(char), 1, ifp);
			if (bDecode)
				c -= 1;
			if (c == ']') 
				fsetpos(ifp, &pos);
		}
	}
}


void Encode(char *lpString, bool bEncode)
{
	if (!bEncode)
		return;

	int Len = strlen(lpString);

	for (int cnt = 0; cnt < Len; cnt++) {
		lpString[cnt] = lpString[cnt]+1;
	}
}


bool EvoParser::WriteParser(char *Filename, bool bBinary)
{

	FILE *ofp;

	if (bBinary) {
		char BinFilename[512];
		strcpy(BinFilename, Filename);
		int Len = strlen(BinFilename);
		strcpy(&BinFilename[Len-3], "pse");

		if ((ofp = fopen(BinFilename, "wb")) == NULL)
			return false;
	}
	else {
			if ((ofp = fopen(Filename, "w")) == NULL)
			return false;
	}

	char String[MAX_PARSER_STRING_LENGTH];
	for (int GroupCnt = 0; GroupCnt < NumGroupKeys; GroupCnt++) {
		wsprintf(String, "[%s]\n", GroupKeys[GroupCnt].GroupName);
		Encode(String, bBinary);
		fprintf(ofp, "%s", String);
		for(int KeyCnt = 0; KeyCnt < GroupKeys[GroupCnt].NumKeys; KeyCnt++) {
			wsprintf(String, "%s\t\t=\t\t%s\n", GroupKeys[GroupCnt].Keys[KeyCnt].KeyName, GroupKeys[GroupCnt].Keys[KeyCnt].KeyValue);
			Encode(String, bBinary);
			fprintf(ofp, "%s", String);
		}
		wsprintf(String, "\n");
		Encode(String, bBinary);
		fprintf(ofp, "%s", String);
	}


	fclose(ofp);
	return true;
}
