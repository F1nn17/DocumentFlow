#include "Library.h"

namespace fs = std::filesystem;
using namespace std;
using namespace Aspose::Words;
using namespace System;

static TCHAR szWindowClass[] = _T("DocFlowApp");
static TCHAR szTitle[] = _T("Document Flow");
HINSTANCE hInst;
HWND editDocument;
HWND labelMessage;
using directory_iterator = std::filesystem::directory_iterator;
string path;
wchar_t fileName[512]{};
wchar_t fileformat[512]{};
string sendFile;
string sendFilePath;
string removeFile;
string openCurrentFile;
unsigned short widthWnd = 1280;
unsigned short heightWnd = 720;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogSendProc(HWND, UINT, WPARAM, LPARAM);
HTREEITEM AddItemtotree(HWND hwndTV, LPWSTR LPSZITEM, HTREEITEM hParent);
BOOL InitTreeViewItems(HWND HWNDTV);
HWND CreateATreeView(HWND hwndParent);
bool dirExists(const std::string& dirName_in);
void CreateFileM(string filepath, HWND tv_parent);
void ReadFile(string path);
void SaveFile(HWND hWnd, string path);
void customSplit(string str, char separator);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

	path = fs::current_path().string();
	path += "\\Documents";

	if (!dirExists(path)) {
		wstring wpath = wstring(path.begin(), path.end());
		LPCWSTR cwpath = wpath.c_str();
		CreateDirectory(cwpath, NULL);
	}

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, NULL);
	wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop"),
			NULL);
		return 1;
	}

	RECT rect = { 0, 0, widthWnd, heightWnd };
	AdjustWindowRectEx(&rect, WS_OVERLAPPED, false, WS_EX_OVERLAPPEDWINDOW);

	HDC DC = GetDC(0);
	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
		((GetDeviceCaps(DC, HORZRES))- widthWnd)/2,
		((GetDeviceCaps(DC, VERTRES)) - heightWnd)/2,
		widthWnd, heightWnd,
		NULL, NULL, hInstance, 0);
	if (!hWnd) {
		MessageBox(NULL,
			_T("Call to CreateWindowEx failed!"),
			_T("Windows Desktop"),
			NULL);

		return 1;
	}

	HWND createFileB = CreateWindow(
		L"BUTTON",
		L"С",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		10,         
		10,         
		32,        
		32,        
		hWnd,     
		(HMENU)1010,       
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      

	HWND addFileB = CreateWindow(
		L"BUTTON",  
		L"A",      
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
		42,         
		10,         
		32,        
		32,        
		hWnd,     
		(HMENU)1007,       
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      

	HWND remFileB = CreateWindow(
		L"BUTTON",  
		L"R",      
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
		74,         
		10,       
		32,       
		32,       
		hWnd,     
		(HMENU)1008,       
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);   

	HWND saveFileB = CreateWindow(
		L"BUTTON",
		L"S",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		200,
		10,
		32,
		32,
		hWnd,
		(HMENU)1111,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);
	HWND ClearEditBoxB = CreateWindow(
		L"BUTTON",
		L"AC",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		868,
		10,
		32,
		32,
		hWnd,
		(HMENU)1112,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);

	editDocument = CreateWindow(
		L"edit",
		NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
		200, 50, 
		700, 600,
		hWnd, 
		NULL, NULL, NULL);

	labelMessage = CreateWindow(L"STATIC",
		NULL, 
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		250, 10, 200, 32, 
		hWnd, (HMENU)900, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
  	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	HDC hdc;
	static HWND tv_parent = NULL;
	LPNMHDR pHdr = reinterpret_cast<LPNMHDR>(lParam);
	NMTREEVIEW* pnmtv = (LPNMTREEVIEW)lParam;
	int dialog{};
	wstring wpathname{};
	wstring viewName{};
	string pathname{};
	string fileRemovePath;
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case 1010:
				dialog = DialogBox(hInst, MAKEINTRESOURCE(NewDocumentWindow), NULL, (DLGPROC)DialogProc);
				if (dialog == 1) {
					CreateFileM(path, tv_parent);
				}
				break;
			case 1008:
				fileRemovePath = path + "\\" + removeFile;
				fstream(fileRemovePath).clear();
				fstream(fileRemovePath).close();
				remove(fileRemovePath.c_str());
				TreeView_DeleteAllItems(tv_parent);
				InitTreeViewItems(tv_parent);
				break;
			case 1007:
				OPENFILENAME ofn;       
				TCHAR szFile[260];      
				HANDLE hf;              
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = L"Text\0*.TXT\0Word Doc\0*.doc\0Word Docx\0*.docx";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = wstring(path.begin(), path.end()).c_str();
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; 
				if (GetOpenFileName(&ofn) == TRUE) {
					try
					{
						fs::path sourceFile = ofn.lpstrFile;
						fs::path targetParent = path;
						auto target = targetParent / sourceFile.filename();
						fs::copy_file(sourceFile, target, fs::copy_options::overwrite_existing);
						TreeView_DeleteAllItems(tv_parent);
						InitTreeViewItems(tv_parent);
					}
					catch (std::exception& e)
					{
						std::cout << e.what();
					}
				}
				break;
			case 1111:
				if(openCurrentFile != "") SaveFile(hWnd,openCurrentFile);
				else SetWindowText(labelMessage, L"Файл не выбран");
				break;
			case 1112:
				SetWindowText(editDocument, L"");
				break;
			default:
				break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		tv_parent = CreateATreeView(hWnd);
		InitTreeViewItems(tv_parent);
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
			case NM_RCLICK:
				TCHAR buffer[32];
				TVITEM item;
				item.hItem = TreeView_GetSelection(tv_parent);
				item.mask = TVIF_TEXT | TVIF_PARAM;
				item.cchTextMax = 32;
				item.pszText = buffer;
				TreeView_GetItem(tv_parent, &item);
				wpathname = wstring(item.pszText);
				pathname = string(wpathname.begin(), wpathname.end());
				sendFilePath = path + "\\" + pathname;
				sendFile = pathname;
				dialog = DialogBox(hInst, MAKEINTRESOURCE(SendDocument), NULL, (DLGPROC)DialogSendProc);
				break;
			case NM_DBLCLK:
				item.hItem = TreeView_GetSelection(tv_parent);
				item.mask = TVIF_TEXT | TVIF_PARAM;
				item.cchTextMax = 32;
				item.pszText = buffer;
				TreeView_GetItem(tv_parent, &item);
				wpathname = wstring(item.pszText);
				pathname = string(wpathname.begin(), wpathname.end());
				openCurrentFile = path + "\\" + pathname;
				viewName = L"Файл: " + wpathname;
				SetWindowText(labelMessage, viewName.c_str());
				ReadFile(openCurrentFile);
				break;
			case NM_CLICK:
				item.hItem = TreeView_GetSelection(tv_parent);
				item.mask = TVIF_TEXT;
				item.cchTextMax = 32;
				item.pszText = buffer;
				TreeView_GetItem(tv_parent, &item);
				wpathname = wstring(item.pszText);
				viewName = L"Файл: " + wpathname;
				removeFile = string(wpathname.begin(), wpathname.end());
				break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

BOOL CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HWND hWndComboBox;
	hWndComboBox = GetDlgItem(hWnd, IDC_FORMATFILE);
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hWnd, IDC_INPUTNAME, fileName, sizeof(fileName));
			GetDlgItemText(hWnd, IDC_FORMATFILE, fileformat, sizeof(fileformat));
			EndDialog(hWnd, 1);
			return TRUE;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	case WM_INITDIALOG:
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)_T("txt"));
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)_T("doc"));
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)_T("docx"));
		return FALSE;
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return FALSE;
	}

	return FALSE;
}

BOOL CALLBACK DialogSendProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wstring currentSignature;
	switch (message) {
		case WM_INITDIALOG:
			SetDlgItemText(hWnd, IDC_SENDFILEVIEW, wstring(sendFile.begin(), sendFile.end()).c_str());
			return FALSE;
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case 1019:
				OPENFILENAME ofn;
				TCHAR szFile[260];
				HANDLE hf;
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = L"sig\0*.sig\0sgn\0*.sgn\0p7s\0*.p7s";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = wstring(path.begin(), path.end()).c_str();
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (GetOpenFileName(&ofn) == TRUE) {
					try
					{
						fs::path sourceFile = ofn.lpstrFile;
						fs::path targetParent = path;
						auto target = targetParent / sourceFile.filename();
						currentSignature = sourceFile.filename();
						fs::copy_file(sourceFile, target, fs::copy_options::overwrite_existing);
					}
					catch (std::exception& e)
					{
						std::cout << e.what();
					}
				}
				SetDlgItemText(hWnd, IDC_SENDSIGN, currentSignature.c_str());
				break;
			case IDSEND:
				EndDialog(hWnd, 1);
				return TRUE;
				break;
			case IDCANCELSEND:
				EndDialog(hWnd, 0);
				return TRUE;
			}
			break;
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return FALSE;
	}
	return FALSE;
}

void CreateFileM(string filepath, HWND tv_parent) {
	wstring wfN(fileName);
	wstring wfF(fileformat);
	string fileName = string(wfN.begin(), wfN.end());
	string fileFormat = string(wfF.begin(), wfF.end());
	string docpath = path + "\\" + fileName + "." + fileFormat;
	wstring wpath = wstring(docpath.begin(), docpath.end());
	ofstream o(docpath);
	TreeView_DeleteAllItems(tv_parent); 
	InitTreeViewItems(tv_parent); 
}

HWND CreateATreeView(HWND hWnd)
{
	HWND hwndTV;
	hwndTV = CreateWindowEx(0,
		WC_TREEVIEW,
		TEXT("Tree View"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
		10,
		50,
		128,
		256,
		hWnd,
		(HMENU)2000,
		hInst,
		NULL);
	return hwndTV;
}

HTREEITEM AddItemtotree(HWND hwndTV, LPWSTR LPSZITEM,HTREEITEM hParent)
{
	TVITEM TVi;
	TVINSERTSTRUCT TVins;
	HTREEITEM hme;
	TVi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_DI_SETITEM | TVIF_PARAM;
	TVi.pszText = LPSZITEM;
	TVins.item = TVi;
	TVins.hInsertAfter = TVI_ROOT;
	if (hParent == NULL)
	{
		TVins.hParent = TVI_ROOT;
	}
	else
	{
		TVins.hParent = hParent;
	}
	hme = TreeView_InsertItem(hwndTV, &TVins);
	return hme;
}

BOOL InitTreeViewItems(HWND HWNDTV)
{
	for (const auto& dirEntry : directory_iterator(path)) {
		wstring name = dirEntry.path().filename().wstring();
		LPWSTR nameLPWSTR = const_cast<LPWSTR>(name.c_str());
		AddItemtotree(HWNDTV, nameLPWSTR, NULL);
	}

	return TRUE;
}

bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false; 

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;  

	return false;
}

vector <string> formats;
void ReadFile(string rpath) {
	try {
		formats.clear();
		string line;
		string result;
		wstring wpath = wstring(rpath.begin(), rpath.end());
		String Spath = Spath.FromWCS(wpath);
		customSplit(rpath, '.');
		if (formats[1] == "docx") {
			auto doc = MakeObject<Document>(Spath);
			auto builder = MakeObject<DocumentBuilder>(doc);
			String text = doc->ToString(SaveFormat::Text);
			wstring wtext = wstring(text.begin(), text.end());
			SetWindowText(editDocument, wtext.c_str());
		}
		else if (formats[1] == "doc") {
			auto doc = MakeObject<Document>(Spath);
			auto builder = MakeObject<DocumentBuilder>(doc);
			String text = doc->ToString(SaveFormat::Text);
			wstring wtext = wstring(text.begin(), text.end());
			SetWindowText(editDocument, wtext.c_str());
		}
		else {
			ifstream in(rpath);
			if (in.is_open())
			{
				while (getline(in, line))
				{
					result += line + "\r\n";
				}
				wstring wline = wstring(result.begin(), result.end());
				LPCWSTR cwline = wline.c_str();
				SetWindowText(editDocument, cwline);
			}
			in.close();
		}
	}
	catch (exception ex) {

		std::cout << ex.what() << endl;
	}
}

void customSplit(string str, char separator) {
	int startIndex = 0, endIndex = 0;
	for (int i = 0; i <= str.size(); i++) {
		if (str[i] == separator || i == str.size()) {
			endIndex = i;
			string temp;
			temp.append(str, startIndex, endIndex - startIndex);
			formats.push_back(temp);
			startIndex = endIndex + 1;
		}
	}
}

std::wstring GetAllTextFromEditControl(HWND hwnd) {
	int textLength = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
	wchar_t* buffer = new wchar_t[textLength + 1];
	SendMessage(hwnd, WM_GETTEXT, textLength + 1, (LPARAM)buffer);
	std::wstring result;
	wchar_t* currentChar = buffer;

	while (*currentChar != '\0') {
		std::wstring line;
		while (*currentChar != '\0' && *currentChar != '\r' && *currentChar != '\n') {
			line += *currentChar;
			++currentChar;
		}
		
		while (*currentChar == '\r' || *currentChar == '\n') {
			++currentChar;
		}
		
		if (!line.empty() || (line.empty() && *(currentChar - 1) == '\n')) {
			result += line + L"\n";
		}
	}
	delete[] buffer;
	return result;
}

void SaveFile(HWND hWnd, string spath) {
	try{
		formats.clear();
		customSplit(spath, '.');
		wstring wspath = wstring(spath.begin(), spath.end());
		String Spath = Spath.FromWCS(wspath);
		std::wstring text = GetAllTextFromEditControl(editDocument);
		if (formats[1] == "doc") {
			auto doc = MakeObject<Document>();
			auto builder = MakeObject<DocumentBuilder>(doc);
			builder->MoveToDocumentStart();
			String writeText = writeText.FromWCS(text);
			builder->Write(writeText);
			doc->Save(Spath, SaveFormat::Doc);
			SetWindowText(editDocument, L"Сохранено!");
		}
		else if (formats[1] == "docx") {
			auto doc = MakeObject<Document>();
			auto builder = MakeObject<DocumentBuilder>(doc);
			builder->MoveToDocumentStart();
			String writeText = writeText.FromWCS(text);
			builder->Write(writeText);
			doc->Save(Spath, SaveFormat::Docx);
			SetWindowText(editDocument, L"Сохранено!");
		}
		else {
			wofstream out;
			out.open(spath, ios::out);
			if (out.is_open())
			{
				out << text;
			}
			out.close();
			SetWindowText(editDocument, L"Сохранено!");
		}
	}
	catch (exception ex) {
		std::cout << ex.what() << endl;
	}
}
