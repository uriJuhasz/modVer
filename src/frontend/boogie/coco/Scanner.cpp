

#include <memory.h>
#include <string.h>
#include "Scanner.h"

namespace frontend {
namespace boogie {



// string handling, wide character


wchar_t* coco_string_create(const wchar_t* value) {
	return coco_string_create(value, 0);
}

wchar_t* coco_string_create(const wchar_t *value, int startIndex) {
	int valueLen = 0;
	int len = 0;

	if (value) {
		valueLen = wcslen(value);
		len = valueLen - startIndex;
	}

	return coco_string_create(value, startIndex, len);
}

wchar_t* coco_string_create(const wchar_t *value, int startIndex, int length) {
	int len = 0;
	wchar_t* data;

	if (value) { len = length; }
	data = new wchar_t[len + 1];
	wcsncpy(data, &(value[startIndex]), len);
	data[len] = 0;

	return data;
}

wchar_t* coco_string_create_upper(const wchar_t* data) {
	if (!data) { return NULL; }

	int dataLen = 0;
	if (data) { dataLen = wcslen(data); }

	wchar_t *newData = new wchar_t[dataLen + 1];

	for (int i = 0; i <= dataLen; i++) {
		if ((L'a' <= data[i]) && (data[i] <= L'z')) {
			newData[i] = data[i] + (L'A' - L'a');
		}
		else { newData[i] = data[i]; }
	}

	newData[dataLen] = L'\0';
	return newData;
}

wchar_t* coco_string_create_lower(const wchar_t* data) {
	if (!data) { return NULL; }
	int dataLen = wcslen(data);
	return coco_string_create_lower(data, 0, dataLen);
}

wchar_t* coco_string_create_lower(const wchar_t* data, int startIndex, int dataLen) {
	if (!data) { return NULL; }

	wchar_t* newData = new wchar_t[dataLen + 1];

	for (int i = 0; i <= dataLen; i++) {
		wchar_t ch = data[startIndex + i];
		if ((L'A' <= ch) && (ch <= L'Z')) {
			newData[i] = ch - (L'A' - L'a');
		}
		else { newData[i] = ch; }
	}
	newData[dataLen] = L'\0';
	return newData;
}

wchar_t* coco_string_create_append(const wchar_t* data1, const wchar_t* data2) {
	wchar_t* data;
	int data1Len = 0;
	int data2Len = 0;

	if (data1) { data1Len = wcslen(data1); }
	if (data2) {data2Len = wcslen(data2); }

	data = new wchar_t[data1Len + data2Len + 1];

	if (data1) { wcscpy(data, data1); }
	if (data2) { wcscpy(data + data1Len, data2); }

	data[data1Len + data2Len] = 0;

	return data;
}

wchar_t* coco_string_create_append(const wchar_t *target, const wchar_t appendix) {
	int targetLen = coco_string_length(target);
	wchar_t* data = new wchar_t[targetLen + 2];
	wcsncpy(data, target, targetLen);
	data[targetLen] = appendix;
	data[targetLen + 1] = 0;
	return data;
}

void coco_string_delete(wchar_t* &data) {
	delete [] data;
	data = NULL;
}

int coco_string_length(const wchar_t* data) {
	if (data) { return wcslen(data); }
	return 0;
}

bool coco_string_endswith(const wchar_t* data, const wchar_t *end) {
	int dataLen = wcslen(data);
	int endLen = wcslen(end);
	return (endLen <= dataLen) && (wcscmp(data + dataLen - endLen, end) == 0);
}

int coco_string_indexof(const wchar_t* data, const wchar_t value) {
	const wchar_t* chr = wcschr(data, value);

	if (chr) { return (chr-data); }
	return -1;
}

int coco_string_lastindexof(const wchar_t* data, const wchar_t value) {
	const wchar_t* chr = wcsrchr(data, value);

	if (chr) { return (chr-data); }
	return -1;
}

void coco_string_merge(wchar_t* &target, const wchar_t* appendix) {
	if (!appendix) { return; }
	wchar_t* data = coco_string_create_append(target, appendix);
	delete [] target;
	target = data;
}

bool coco_string_equal(const wchar_t* data1, const wchar_t* data2) {
	return wcscmp( data1, data2 ) == 0;
}

int coco_string_compareto(const wchar_t* data1, const wchar_t* data2) {
	return wcscmp(data1, data2);
}

int coco_string_hash(const wchar_t *data) {
	int h = 0;
	if (!data) { return 0; }
	while (*data != 0) {
		h = (h * 7) ^ *data;
		++data;
	}
	if (h < 0) { h = -h; }
	return h;
}

// string handling, ascii character

wchar_t* coco_string_create(const char* value) {
	int len = 0;
	if (value) { len = strlen(value); }
	wchar_t* data = new wchar_t[len + 1];
	for (int i = 0; i < len; ++i) { data[i] = (wchar_t) value[i]; }
	data[len] = 0;
	return data;
}

char* coco_string_create_char(const wchar_t *value) {
	int len = coco_string_length(value);
	char *res = new char[len + 1];
	for (int i = 0; i < len; ++i) { res[i] = (char) value[i]; }
	res[len] = 0;
	return res;
}

void coco_string_delete(char* &data) {
	delete [] data;
	data = NULL;
}


Token::Token() {
	kind = 0;
	pos  = 0;
	col  = 0;
	line = 0;
	val  = NULL;
	next = NULL;
}

Token::~Token() {
	coco_string_delete(val);
}

Buffer::Buffer(FILE* s, bool isUserStream) {
// ensure binary read on windows
#if _MSC_VER >= 1300
	_setmode(_fileno(s), _O_BINARY);
#endif
	stream = s; this->isUserStream = isUserStream;
	if (CanSeek()) {
		fseek(s, 0, SEEK_END);
		fileLen = ftell(s);
		fseek(s, 0, SEEK_SET);
		bufLen = (fileLen < COCO_MAX_BUFFER_LENGTH) ? fileLen : COCO_MAX_BUFFER_LENGTH;
		bufStart = INT_MAX; // nothing in the buffer so far
	} else {
		fileLen = bufLen = bufStart = 0;
	}
	bufCapacity = (bufLen>0) ? bufLen : COCO_MIN_BUFFER_LENGTH;
	buf = new unsigned char[bufCapacity];	
	if (fileLen > 0) SetPos(0);          // setup  buffer to position 0 (start)
	else bufPos = 0; // index 0 is already after the file, thus Pos = 0 is invalid
	if (bufLen == fileLen && CanSeek()) Close();
}

Buffer::Buffer(Buffer *b) {
	buf = b->buf;
	bufCapacity = b->bufCapacity;
	b->buf = NULL;
	bufStart = b->bufStart;
	bufLen = b->bufLen;
	fileLen = b->fileLen;
	bufPos = b->bufPos;
	stream = b->stream;
	b->stream = NULL;
	isUserStream = b->isUserStream;
}

Buffer::Buffer(const unsigned char* buf, int len) {
	this->buf = new unsigned char[len];
	memcpy(this->buf, buf, len*sizeof(unsigned char));
	bufStart = 0;
	bufCapacity = bufLen = len;
	fileLen = len;
	bufPos = 0;
	stream = NULL;
}

Buffer::~Buffer() {
	Close(); 
	if (buf != NULL) {
		delete [] buf;
		buf = NULL;
	}
}

void Buffer::Close() {
	if (!isUserStream && stream != NULL) {
		fclose(stream);
		stream = NULL;
	}
}

int Buffer::Read() {
	if (bufPos < bufLen) {
		return buf[bufPos++];
	} else if (GetPos() < fileLen) {
		SetPos(GetPos()); // shift buffer start to Pos
		return buf[bufPos++];
	} else if ((stream != NULL) && !CanSeek() && (ReadNextStreamChunk() > 0)) {
		return buf[bufPos++];
	} else {
		return EoF;
	}
}

int Buffer::Peek() {
	int curPos = GetPos();
	int ch = Read();
	SetPos(curPos);
	return ch;
}

// beg .. begin, zero-based, inclusive, in byte
// end .. end, zero-based, exclusive, in byte
wchar_t* Buffer::GetString(int beg, int end) {
	int len = 0;
	wchar_t *buf = new wchar_t[end - beg];
	int oldPos = GetPos();
	SetPos(beg);
	while (GetPos() < end) buf[len++] = (wchar_t) Read();
	SetPos(oldPos);
	wchar_t *res = coco_string_create(buf, 0, len);
	coco_string_delete(buf);
	return res;
}

int Buffer::GetPos() {
	return bufPos + bufStart;
}

void Buffer::SetPos(int value) {
	if ((value >= fileLen) && (stream != NULL) && !CanSeek()) {
		// Wanted position is after buffer and the stream
		// is not seek-able e.g. network or console,
		// thus we have to read the stream manually till
		// the wanted position is in sight.
		while ((value >= fileLen) && (ReadNextStreamChunk() > 0));
	}

	if ((value < 0) || (value > fileLen)) {
		wprintf(L"--- buffer out of bounds access, position: %d\n", value);
		exit(1);
	}

	if ((value >= bufStart) && (value < (bufStart + bufLen))) { // already in buffer
		bufPos = value - bufStart;
	} else if (stream != NULL) { // must be swapped in
		fseek(stream, value, SEEK_SET);
		bufLen = fread(buf, sizeof(unsigned char), bufCapacity, stream);
		bufStart = value; bufPos = 0;
	} else {
		bufPos = fileLen - bufStart; // make Pos return fileLen
	}
}

// Read the next chunk of bytes from the stream, increases the buffer
// if needed and updates the fields fileLen and bufLen.
// Returns the number of bytes read.
int Buffer::ReadNextStreamChunk() {
	int free = bufCapacity - bufLen;
	if (free == 0) {
		// in the case of a growing input stream
		// we can neither seek in the stream, nor can we
		// foresee the maximum length, thus we must adapt
		// the buffer size on demand.
		bufCapacity = bufLen * 2;
		unsigned char *newBuf = new unsigned char[bufCapacity];
		memcpy(newBuf, buf, bufLen*sizeof(unsigned char));
		delete [] buf;
		buf = newBuf;
		free = bufLen;
	}
	int read = fread(buf + bufLen, sizeof(unsigned char), free, stream);
	if (read > 0) {
		fileLen = bufLen = (bufLen + read);
		return read;
	}
	// end of stream reached
	return 0;
}

bool Buffer::CanSeek() {
	return (stream != NULL) && (ftell(stream) != -1);
}

int UTF8Buffer::Read() {
	int ch;
	do {
		ch = Buffer::Read();
		// until we find a utf8 start (0xxxxxxx or 11xxxxxx)
	} while ((ch >= 128) && ((ch & 0xC0) != 0xC0) && (ch != EoF));
	if (ch < 128 || ch == EoF) {
		// nothing to do, first 127 chars are the same in ascii and utf8
		// 0xxxxxxx or end of file character
	} else if ((ch & 0xF0) == 0xF0) {
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		int c1 = ch & 0x07; ch = Buffer::Read();
		int c2 = ch & 0x3F; ch = Buffer::Read();
		int c3 = ch & 0x3F; ch = Buffer::Read();
		int c4 = ch & 0x3F;
		ch = (((((c1 << 6) | c2) << 6) | c3) << 6) | c4;
	} else if ((ch & 0xE0) == 0xE0) {
		// 1110xxxx 10xxxxxx 10xxxxxx
		int c1 = ch & 0x0F; ch = Buffer::Read();
		int c2 = ch & 0x3F; ch = Buffer::Read();
		int c3 = ch & 0x3F;
		ch = (((c1 << 6) | c2) << 6) | c3;
	} else if ((ch & 0xC0) == 0xC0) {
		// 110xxxxx 10xxxxxx
		int c1 = ch & 0x1F; ch = Buffer::Read();
		int c2 = ch & 0x3F;
		ch = (c1 << 6) | c2;
	}
	return ch;
}

Scanner::Scanner(const unsigned char* buf, int len) {
	buffer = new Buffer(buf, len);
	Init();
}

Scanner::Scanner(const wchar_t* fileName) {
	FILE* stream;
	char *chFileName = coco_string_create_char(fileName);
	if ((stream = fopen(chFileName, "rb")) == NULL) {
		wprintf(L"--- Cannot open file %ls\n", fileName);
		exit(1);
	}
	coco_string_delete(chFileName);
	buffer = new Buffer(stream, false);
	Init();
}

Scanner::Scanner(FILE* s) {
	buffer = new Buffer(s, true);
	Init();
}

Scanner::~Scanner() {
	char* cur = (char*) firstHeap;

	while(cur != NULL) {
		cur = *(char**) (cur + COCO_HEAP_BLOCK_SIZE);
		free(firstHeap);
		firstHeap = cur;
	}
	delete [] tval;
	delete buffer;
}

void Scanner::Init() {
	EOL    = '\n';
	eofSym = 0;
	maxT = 97;
	noSym = 97;
	int i;
	for (i = 35; i <= 36; ++i) start.set(i, 2);
	for (i = 39; i <= 39; ++i) start.set(i, 2);
	for (i = 46; i <= 46; ++i) start.set(i, 2);
	for (i = 63; i <= 63; ++i) start.set(i, 2);
	for (i = 65; i <= 90; ++i) start.set(i, 2);
	for (i = 94; i <= 122; ++i) start.set(i, 2);
	for (i = 126; i <= 126; ++i) start.set(i, 2);
	for (i = 49; i <= 57; ++i) start.set(i, 45);
	for (i = 34; i <= 34; ++i) start.set(i, 6);
	start.set(92, 1);
	start.set(45, 87);
	start.set(48, 46);
	start.set(59, 52);
	start.set(40, 53);
	start.set(41, 54);
	start.set(58, 88);
	start.set(44, 55);
	start.set(91, 56);
	start.set(93, 57);
	start.set(60, 89);
	start.set(62, 90);
	start.set(123, 58);
	start.set(125, 91);
	start.set(61, 92);
	start.set(42, 93);
	start.set(124, 94);
	start.set(8660, 61);
	start.set(8658, 63);
	start.set(8656, 64);
	start.set(38, 65);
	start.set(8743, 67);
	start.set(8744, 69);
	start.set(33, 95);
	start.set(8800, 73);
	start.set(8804, 74);
	start.set(8805, 75);
	start.set(43, 96);
	start.set(47, 77);
	start.set(172, 79);
	start.set(8704, 82);
	start.set(8707, 83);
	start.set(955, 84);
	start.set(8226, 86);
		start.set(Buffer::EoF, -1);
	keywords.set(L"var", 8);
	keywords.set(L"where", 14);
	keywords.set(L"int", 15);
	keywords.set(L"real", 16);
	keywords.set(L"bool", 17);
	keywords.set(L"const", 22);
	keywords.set(L"unique", 23);
	keywords.set(L"extends", 24);
	keywords.set(L"complete", 25);
	keywords.set(L"function", 26);
	keywords.set(L"returns", 27);
	keywords.set(L"axiom", 30);
	keywords.set(L"type", 31);
	keywords.set(L"procedure", 33);
	keywords.set(L"implementation", 34);
	keywords.set(L"modifies", 35);
	keywords.set(L"free", 36);
	keywords.set(L"requires", 37);
	keywords.set(L"ensures", 38);
	keywords.set(L"goto", 39);
	keywords.set(L"return", 40);
	keywords.set(L"if", 41);
	keywords.set(L"else", 42);
	keywords.set(L"while", 43);
	keywords.set(L"invariant", 44);
	keywords.set(L"break", 46);
	keywords.set(L"assert", 47);
	keywords.set(L"assume", 48);
	keywords.set(L"havoc", 49);
	keywords.set(L"yield", 50);
	keywords.set(L"async", 52);
	keywords.set(L"call", 53);
	keywords.set(L"par", 54);
	keywords.set(L"div", 77);
	keywords.set(L"mod", 78);
	keywords.set(L"false", 83);
	keywords.set(L"true", 84);
	keywords.set(L"old", 85);
	keywords.set(L"then", 88);
	keywords.set(L"forall", 89);
	keywords.set(L"exists", 91);
	keywords.set(L"lambda", 93);


	tvalLength = 128;
	tval = new wchar_t[tvalLength]; // text of current token

	// COCO_HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	heap = malloc(COCO_HEAP_BLOCK_SIZE + sizeof(void*));
	firstHeap = heap;
	heapEnd = (void**) (((char*) heap) + COCO_HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heapTop = heap;
	if (sizeof(Token) > COCO_HEAP_BLOCK_SIZE) {
		wprintf(L"--- Too small COCO_HEAP_BLOCK_SIZE\n");
		exit(1);
	}

	pos = -1; line = 1; col = 0; charPos = -1;
	oldEols = 0;
	NextCh();
	if (ch == 0xEF) { // check optional byte order mark for UTF-8
		NextCh(); int ch1 = ch;
		NextCh(); int ch2 = ch;
		if (ch1 != 0xBB || ch2 != 0xBF) {
			wprintf(L"Illegal byte order mark at start of file");
			exit(1);
		}
		Buffer *oldBuf = buffer;
		buffer = new UTF8Buffer(buffer); col = 0; charPos = -1;
		delete oldBuf; oldBuf = NULL;
		NextCh();
	}


	pt = tokens = CreateToken(); // first token is a dummy
}

void Scanner::NextCh() {
	if (oldEols > 0) { ch = EOL; oldEols--; }
	else {
		pos = buffer->GetPos();
		// buffer reads unicode chars, if UTF8 has been detected
		ch = buffer->Read(); col++; charPos++;
		// replace isolated '\r' by '\n' in order to make
		// eol handling uniform across Windows, Unix and Mac
		if (ch == L'\r' && buffer->Peek() != L'\n') ch = EOL;
		if (ch == EOL) { line++; col = 0; }
	}

}

void Scanner::AddCh() {
	if (tlen >= tvalLength) {
		tvalLength *= 2;
		wchar_t *newBuf = new wchar_t[tvalLength];
		memcpy(newBuf, tval, tlen*sizeof(wchar_t));
		delete [] tval;
		tval = newBuf;
	}
	if (ch != Buffer::EoF) {
		tval[tlen++] = ch;
		NextCh();
	}
}


bool Scanner::Comment0() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == L'/') {
		NextCh();
		for(;;) {
			if (ch == 10) {
				level--;
				if (level == 0) { oldEols = line - line0; NextCh(); return true; }
				NextCh();
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}

bool Scanner::Comment1() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == L'*') {
		NextCh();
		for(;;) {
			if (ch == L'*') {
				NextCh();
				if (ch == L'/') {
					level--;
					if (level == 0) { oldEols = line - line0; NextCh(); return true; }
					NextCh();
				}
			} else if (ch == L'/') {
				NextCh();
				if (ch == L'*') {
					level++; NextCh();
				}
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}


void Scanner::CreateHeapBlock() {
	void* newHeap;
	char* cur = (char*) firstHeap;

	while(((char*) tokens < cur) || ((char*) tokens > (cur + COCO_HEAP_BLOCK_SIZE))) {
		cur = *((char**) (cur + COCO_HEAP_BLOCK_SIZE));
		free(firstHeap);
		firstHeap = cur;
	}

	// COCO_HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	newHeap = malloc(COCO_HEAP_BLOCK_SIZE + sizeof(void*));
	*heapEnd = newHeap;
	heapEnd = (void**) (((char*) newHeap) + COCO_HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heap = newHeap;
	heapTop = heap;
}

Token* Scanner::CreateToken() {
	Token *t;
	if (((char*) heapTop + (int) sizeof(Token)) >= (char*) heapEnd) {
		CreateHeapBlock();
	}
	t = (Token*) heapTop;
	heapTop = (void*) ((char*) heapTop + sizeof(Token));
	t->val = NULL;
	t->next = NULL;
	return t;
}

void Scanner::AppendVal(Token *t) {
	int reqMem = (tlen + 1) * sizeof(wchar_t);
	if (((char*) heapTop + reqMem) >= (char*) heapEnd) {
		if (reqMem > COCO_HEAP_BLOCK_SIZE) {
			wprintf(L"--- Too long token value\n");
			exit(1);
		}
		CreateHeapBlock();
	}
	t->val = (wchar_t*) heapTop;
	heapTop = (void*) ((char*) heapTop + reqMem);

	wcsncpy(t->val, tval, tlen);
	t->val[tlen] = L'\0';
}

Token* Scanner::NextToken() {
	while (ch == ' ' ||
			(ch >= 9 && ch <= 10) || ch == 13
	) NextCh();
	if ((ch == L'/' && Comment0()) || (ch == L'/' && Comment1())) return NextToken();
	int recKind = noSym;
	int recEnd = pos;
	t = CreateToken();
	t->pos = pos; t->col = col; t->line = line; t->charPos = charPos;
	int state = start.state(ch);
	tlen = 0; AddCh();

	switch (state) {
		case -1: { t->kind = eofSym; break; } // NextCh already done
		case 0: {
			case_0:
			if (recKind != noSym) {
				tlen = recEnd - t->pos;
				SetScannerBehindT();
			}
			t->kind = recKind; break;
		} // NextCh already done
		case 1:
			if ((ch >= L'#' && ch <= L'$') || ch == 39 || ch == L'.' || ch == L'?' || (ch >= L'A' && ch <= L'Z') || (ch >= L'^' && ch <= L'z') || ch == L'~') {AddCh(); goto case_2;}
			else {goto case_0;}
		case 2:
			case_2:
			recEnd = pos; recKind = 1;
			if ((ch >= L'#' && ch <= L'$') || ch == 39 || ch == L'.' || (ch >= L'0' && ch <= L'9') || ch == L'?' || (ch >= L'A' && ch <= L'Z') || (ch >= L'^' && ch <= L'z') || ch == L'~') {AddCh(); goto case_2;}
			else {t->kind = 1; wchar_t *literal = coco_string_create(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 3:
			case_3:
			if (ch == L'v') {AddCh(); goto case_4;}
			else {goto case_0;}
		case 4:
			case_4:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_5;}
			else {goto case_0;}
		case 5:
			case_5:
			recEnd = pos; recKind = 2;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_5;}
			else {t->kind = 2; break;}
		case 6:
			case_6:
			if (ch == L'"') {AddCh(); goto case_7;}
			else if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L'!') || (ch >= L'#' && ch <= L'[') || (ch >= L']' && ch <= 65535)) {AddCh(); goto case_6;}
			else if (ch == 92) {AddCh(); goto case_47;}
			else {goto case_0;}
		case 7:
			case_7:
			{t->kind = 4; break;}
		case 8:
			case_8:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_9;}
			else {goto case_0;}
		case 9:
			case_9:
			recEnd = pos; recKind = 6;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_9;}
			else if (ch == L'e') {AddCh(); goto case_10;}
			else {t->kind = 6; break;}
		case 10:
			case_10:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_12;}
			else if (ch == L'-') {AddCh(); goto case_11;}
			else {goto case_0;}
		case 11:
			case_11:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_12;}
			else {goto case_0;}
		case 12:
			case_12:
			recEnd = pos; recKind = 6;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_12;}
			else {t->kind = 6; break;}
		case 13:
			case_13:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_13;}
			else if (ch == L'e') {AddCh(); goto case_14;}
			else {goto case_0;}
		case 14:
			case_14:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_16;}
			else if (ch == L'-') {AddCh(); goto case_15;}
			else {goto case_0;}
		case 15:
			case_15:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_16;}
			else {goto case_0;}
		case 16:
			case_16:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_16;}
			else if (ch == L'f') {AddCh(); goto case_17;}
			else {goto case_0;}
		case 17:
			case_17:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_18;}
			else {goto case_0;}
		case 18:
			case_18:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_18;}
			else if (ch == L'e') {AddCh(); goto case_19;}
			else {goto case_0;}
		case 19:
			case_19:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_20;}
			else {goto case_0;}
		case 20:
			case_20:
			recEnd = pos; recKind = 7;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_20;}
			else {t->kind = 7; break;}
		case 21:
			case_21:
			if (ch == L'a') {AddCh(); goto case_22;}
			else {goto case_0;}
		case 22:
			case_22:
			if (ch == L'N') {AddCh(); goto case_23;}
			else {goto case_0;}
		case 23:
			case_23:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_24;}
			else {goto case_0;}
		case 24:
			case_24:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_24;}
			else if (ch == L'e') {AddCh(); goto case_25;}
			else {goto case_0;}
		case 25:
			case_25:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_26;}
			else {goto case_0;}
		case 26:
			case_26:
			recEnd = pos; recKind = 7;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_26;}
			else {t->kind = 7; break;}
		case 27:
			case_27:
			if (ch == L'a') {AddCh(); goto case_28;}
			else {goto case_0;}
		case 28:
			case_28:
			if (ch == L'n') {AddCh(); goto case_29;}
			else {goto case_0;}
		case 29:
			case_29:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_30;}
			else {goto case_0;}
		case 30:
			case_30:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_30;}
			else if (ch == L'e') {AddCh(); goto case_31;}
			else {goto case_0;}
		case 31:
			case_31:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_32;}
			else {goto case_0;}
		case 32:
			case_32:
			recEnd = pos; recKind = 7;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_32;}
			else {t->kind = 7; break;}
		case 33:
			case_33:
			if (ch == L'o') {AddCh(); goto case_34;}
			else {goto case_0;}
		case 34:
			case_34:
			if (ch == L'o') {AddCh(); goto case_35;}
			else {goto case_0;}
		case 35:
			case_35:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_36;}
			else {goto case_0;}
		case 36:
			case_36:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_36;}
			else if (ch == L'e') {AddCh(); goto case_37;}
			else {goto case_0;}
		case 37:
			case_37:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_38;}
			else {goto case_0;}
		case 38:
			case_38:
			recEnd = pos; recKind = 7;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_38;}
			else {t->kind = 7; break;}
		case 39:
			case_39:
			if (ch == L'o') {AddCh(); goto case_40;}
			else {goto case_0;}
		case 40:
			case_40:
			if (ch == L'o') {AddCh(); goto case_41;}
			else {goto case_0;}
		case 41:
			case_41:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_42;}
			else {goto case_0;}
		case 42:
			case_42:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_42;}
			else if (ch == L'e') {AddCh(); goto case_43;}
			else {goto case_0;}
		case 43:
			case_43:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_44;}
			else {goto case_0;}
		case 44:
			case_44:
			recEnd = pos; recKind = 7;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_44;}
			else {t->kind = 7; break;}
		case 45:
			case_45:
			recEnd = pos; recKind = 3;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_45;}
			else if (ch == L'b') {AddCh(); goto case_3;}
			else if (ch == L'e') {AddCh(); goto case_48;}
			else if (ch == L'.') {AddCh(); goto case_8;}
			else {t->kind = 3; break;}
		case 46:
			recEnd = pos; recKind = 3;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_45;}
			else if (ch == L'b') {AddCh(); goto case_3;}
			else if (ch == L'e') {AddCh(); goto case_48;}
			else if (ch == L'.') {AddCh(); goto case_8;}
			else if (ch == L'N') {AddCh(); goto case_21;}
			else if (ch == L'n') {AddCh(); goto case_27;}
			else if (ch == L'+') {AddCh(); goto case_33;}
			else if (ch == L'-') {AddCh(); goto case_39;}
			else {t->kind = 3; break;}
		case 47:
			case_47:
			if (ch == L'"') {AddCh(); goto case_49;}
			else if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L'!') || (ch >= L'#' && ch <= L'[') || (ch >= L']' && ch <= 65535)) {AddCh(); goto case_6;}
			else if (ch == 92) {AddCh(); goto case_47;}
			else {goto case_0;}
		case 48:
			case_48:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_50;}
			else if (ch == L'-') {AddCh(); goto case_51;}
			else {goto case_0;}
		case 49:
			case_49:
			recEnd = pos; recKind = 4;
			if (ch == L'"') {AddCh(); goto case_7;}
			else if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= L'!') || (ch >= L'#' && ch <= L'[') || (ch >= L']' && ch <= 65535)) {AddCh(); goto case_6;}
			else if (ch == 92) {AddCh(); goto case_47;}
			else {t->kind = 4; break;}
		case 50:
			case_50:
			recEnd = pos; recKind = 5;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_50;}
			else if (ch == L'f') {AddCh(); goto case_17;}
			else {t->kind = 5; break;}
		case 51:
			case_51:
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_50;}
			else {goto case_0;}
		case 52:
			{t->kind = 9; break;}
		case 53:
			{t->kind = 10; break;}
		case 54:
			{t->kind = 11; break;}
		case 55:
			{t->kind = 13; break;}
		case 56:
			{t->kind = 18; break;}
		case 57:
			{t->kind = 19; break;}
		case 58:
			{t->kind = 28; break;}
		case 59:
			case_59:
			{t->kind = 51; break;}
		case 60:
			case_60:
			{t->kind = 56; break;}
		case 61:
			{t->kind = 57; break;}
		case 62:
			case_62:
			{t->kind = 58; break;}
		case 63:
			{t->kind = 59; break;}
		case 64:
			{t->kind = 61; break;}
		case 65:
			if (ch == L'&') {AddCh(); goto case_66;}
			else {goto case_0;}
		case 66:
			case_66:
			{t->kind = 62; break;}
		case 67:
			{t->kind = 63; break;}
		case 68:
			case_68:
			{t->kind = 64; break;}
		case 69:
			{t->kind = 65; break;}
		case 70:
			case_70:
			{t->kind = 68; break;}
		case 71:
			case_71:
			{t->kind = 69; break;}
		case 72:
			case_72:
			{t->kind = 70; break;}
		case 73:
			{t->kind = 71; break;}
		case 74:
			{t->kind = 72; break;}
		case 75:
			{t->kind = 73; break;}
		case 76:
			case_76:
			{t->kind = 74; break;}
		case 77:
			{t->kind = 79; break;}
		case 78:
			case_78:
			{t->kind = 80; break;}
		case 79:
			{t->kind = 82; break;}
		case 80:
			case_80:
			{t->kind = 86; break;}
		case 81:
			case_81:
			{t->kind = 87; break;}
		case 82:
			{t->kind = 90; break;}
		case 83:
			{t->kind = 92; break;}
		case 84:
			{t->kind = 94; break;}
		case 85:
			case_85:
			{t->kind = 95; break;}
		case 86:
			{t->kind = 96; break;}
		case 87:
			recEnd = pos; recKind = 76;
			if ((ch >= L'0' && ch <= L'9')) {AddCh(); goto case_13;}
			else {t->kind = 76; break;}
		case 88:
			recEnd = pos; recKind = 12;
			if (ch == L'=') {AddCh(); goto case_59;}
			else if (ch == L':') {AddCh(); goto case_85;}
			else {t->kind = 12; break;}
		case 89:
			recEnd = pos; recKind = 20;
			if (ch == L'=') {AddCh(); goto case_97;}
			else if (ch == L':') {AddCh(); goto case_72;}
			else {t->kind = 20; break;}
		case 90:
			recEnd = pos; recKind = 21;
			if (ch == L'=') {AddCh(); goto case_70;}
			else {t->kind = 21; break;}
		case 91:
			recEnd = pos; recKind = 29;
			if (ch == L'|') {AddCh(); goto case_81;}
			else {t->kind = 29; break;}
		case 92:
			recEnd = pos; recKind = 32;
			if (ch == L'=') {AddCh(); goto case_98;}
			else {t->kind = 32; break;}
		case 93:
			recEnd = pos; recKind = 45;
			if (ch == L'*') {AddCh(); goto case_78;}
			else {t->kind = 45; break;}
		case 94:
			recEnd = pos; recKind = 55;
			if (ch == L'|') {AddCh(); goto case_68;}
			else if (ch == L'{') {AddCh(); goto case_80;}
			else {t->kind = 55; break;}
		case 95:
			recEnd = pos; recKind = 81;
			if (ch == L'=') {AddCh(); goto case_71;}
			else {t->kind = 81; break;}
		case 96:
			recEnd = pos; recKind = 75;
			if (ch == L'+') {AddCh(); goto case_76;}
			else {t->kind = 75; break;}
		case 97:
			case_97:
			recEnd = pos; recKind = 67;
			if (ch == L'=') {AddCh(); goto case_99;}
			else {t->kind = 67; break;}
		case 98:
			case_98:
			recEnd = pos; recKind = 66;
			if (ch == L'>') {AddCh(); goto case_62;}
			else {t->kind = 66; break;}
		case 99:
			case_99:
			recEnd = pos; recKind = 60;
			if (ch == L'>') {AddCh(); goto case_60;}
			else {t->kind = 60; break;}

	}
	AppendVal(t);
	return t;
}

void Scanner::SetScannerBehindT() {
	buffer->SetPos(t->pos);
	NextCh();
	line = t->line; col = t->col; charPos = t->charPos;
	for (int i = 0; i < tlen; i++) NextCh();
}

// get the next token (possibly a token already seen during peeking)
Token* Scanner::Scan() {
	if (tokens->next == NULL) {
		return pt = tokens = NextToken();
	} else {
		pt = tokens = tokens->next;
		return tokens;
	}
}

// peek for the next token, ignore pragmas
Token* Scanner::Peek() {
	do {
		if (pt->next == NULL) {
			pt->next = NextToken();
		}
		pt = pt->next;
	} while (pt->kind > maxT); // skip pragmas

	return pt;
}

// make sure that peeking starts at the current scan position
void Scanner::ResetPeek() {
	pt = tokens;
}

} // namespace
} // namespace

