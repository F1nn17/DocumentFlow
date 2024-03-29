#include "Library.h"

using namespace Spire::Doc;
using namespace Spire::Pdf;
using namespace std;
namespace fs = std::filesystem;

static TCHAR szWindowClass[] = _T("DocFlowApp");
static TCHAR szTitle[] = _T("Document Flow");
HINSTANCE hInst;
HWND editDocument;
using directory_iterator = std::filesystem::directory_iterator;
std::string path;
wchar_t fileName[512]{};
wchar_t fileformat[512]{};
string sendFile;
string sendFilePath;
//window size
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
void customSplit(string str, char separator);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

	// �������� ����
	path = fs::current_path().string();
	path += "\\Documents";
	if (!dirExists(path)) {
		wstring wpath = wstring(path.begin(), path.end());
		LPCWSTR cwpath = wpath.c_str();
		CreateDirectory(cwpath, NULL);
	}
	//�������� (���������) � ������� ����
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

	//����������� ����
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop"),
			NULL);
		return 1;
	}

	RECT rect = { 0, 0, widthWnd, heightWnd };
	AdjustWindowRectEx(&rect, WS_OVERLAPPED, false, WS_EX_OVERLAPPEDWINDOW);

	//������� CreateWindowEX
	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass, szTitle,
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME /* & ~WS_MAXIMIZEBOX*/,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, hInstance, 0);
	if (!hWnd) {
		MessageBox(NULL,
			_T("Call to CreateWindowEx failed!"),
			_T("Windows Desktop"),
			NULL);

		return 1;
	}

	//������� ���������
	HWND createFileB = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"�",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // ����� 
		10,         // x position (������� �)
		10,         // y position (������� �)
		32,        // Button width (������)
		32,        // Button height (������)
		hWnd,     // ������������ ���� (������� ����)
		(HMENU)1010,       //Menu.
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

	HWND addFileB = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"A",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // ����� 
		42,         // x position (������� �)
		10,         // y position (������� �)
		32,        // Button width (������)
		32,        // Button height (������)
		hWnd,     // ������������ ���� (������� ����)
		(HMENU)1007,       //Menu.
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

	HWND remileB = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"R",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // ����� 
		74,         // x position (������� �)
		10,         // y position (������� �)
		32,        // Button width (������)
		32,        // Button height (������)
		hWnd,     // ������������ ���� (������� ����)
		(HMENU)1008,       //Menu.
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.
    // Pointer not needed.

	//���� ��������������
	//Add edit box for nickname  
	editDocument = CreateWindow(
		L"edit",
		NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
		200, 50, 
		700, 600,
		hWnd, 
		NULL, NULL, NULL);


	//���������� ����
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//������������� ����������
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	//��������� ���������
	PAINTSTRUCT ps;
	HDC hdc;
	static HWND tv_parent = NULL;
	LPNMHDR pHdr = reinterpret_cast<LPNMHDR>(lParam);
	NMTREEVIEW* pnmtv = (LPNMTREEVIEW)lParam;
	int dialog{};

	wstring wpathname{};
	string pathname{};

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
			case 1002:
				break;
			case 1007:
				OPENFILENAME ofn;       // common dialog box structure
				TCHAR szFile[260];       // buffer for file name
				HANDLE hf;              // file handle

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = szFile;
				// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
				// use the contents of szFile to initialize itself.
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = L"Text\0*.TXT\0Word Doc\0*.doc\0Word Docx\0*.docx\0Document pdf\0*.pdf";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = wstring(path.begin(), path.end()).c_str();
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				// Display the Open dialog box. 

				if (GetOpenFileName(&ofn) == TRUE)
					hf = CreateFile(ofn.lpstrFile,
						GENERIC_READ,
						0,
						(LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						(HANDLE)NULL);
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
				if (dialog == 1) {
					
				}

				break;
			case NM_DBLCLK:
				item.hItem = TreeView_GetSelection(tv_parent);
				item.mask = TVIF_TEXT | TVIF_PARAM;
				item.cchTextMax = 32;
				item.pszText = buffer;
				TreeView_GetItem(tv_parent, &item);
				wpathname = wstring(item.pszText);
				pathname = string(wpathname.begin(), wpathname.end());
				ReadFile(path+"\\"+pathname);
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
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, (LPARAM)_T("pdf"));
		return FALSE;
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return FALSE;
	}

	return FALSE;
}

BOOL CALLBACK DialogSendProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	{

	switch(message) {
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_SENDFILEVIEW, wstring(sendFile.begin(), sendFile.end()).c_str());
		return FALSE;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
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
	if (fileformat == L"pdf") {
		PdfDocument* docPdf = new PdfDocument();
		boost::intrusive_ptr<PdfPageBase> page = docPdf->GetPages()->Add();
		docPdf->SaveToFile(wpath.c_str());
		docPdf->Dispose();
		delete docPdf;
	}
	else {
		ofstream o(docpath);
	}
	TreeView_DeleteAllItems(tv_parent); //������� ������
	InitTreeViewItems(tv_parent); // ������������� ������
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
	TVITEM TVi; // Specify or receive properties of tree
	TVINSERTSTRUCT TVins; // contains information for adding new projects to tree control. This structure is used for TVM_INSERTITEM messages
	HTREEITEM hme;
	// Setting the parameters of Item
	TVi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_DI_SETITEM | TVIF_PARAM;
	TVi.pszText = LPSZITEM;
	// Fill the Struct structure
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
	// Call the key TreeView_InsertItem function
	hme = TreeView_InsertItem(hwndTV, &TVins);
	return hme;
}

BOOL InitTreeViewItems(HWND HWNDTV) // Create multiple nodes
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
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

vector <string> formats;
void ReadFile(string path) {
	try {
		formats.clear();
		string line;
		string result;
		wstring wpath = wstring(path.begin(), path.end());
		Document* document = NULL;
		PdfDocument* docPDF = NULL;
		customSplit(path, '.');
		if (formats[1] == "docx") {
			document = new Document();
			document->LoadFromFile(wpath.c_str());
			wstring text = document->GetText();
			SetWindowText(editDocument, text.c_str());
		}
		else if (formats[1] == "doc") {
			document = new Document();
			document->LoadFromFile(wpath.c_str());
			wstring text = document->GetText();
			SetWindowText(editDocument, text.c_str());
		}
		else if (formats[1] == "pdf") {
			/*docPDF = new PdfDocument();
			docPDF->LoadFromFile(wpath.c_str(), 0);*/
			//wstring text;
			/*for (int i = 0; i < docPDF->GetPages()->GetCount(); i++)
			{
				boost::intrusive_ptr<PdfPageBase> page = docPDF->GetPages()->GetItem(i);
				text += (page->ExtractText());
			}
			SetWindowText(editDocument, text.c_str());*/
		}
		else {
			ifstream in(path); // �������� ���� ��� ������
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
			in.close();     // ��������� ����
		}
		if (document != NULL) {
			document->Close();
			delete document;
		}
		if (docPDF != NULL) {
			docPDF->Close();
			delete docPDF;
		}
	}
	catch (exception ex) {

		if (AllocConsole() == TRUE)
		{
			WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), ex.what(), strlen(ex.what()), NULL, NULL);

			FreeConsole();
		}

	}
}

// Create custom split() function.  
void customSplit(string str, char separator) {
	int startIndex = 0, endIndex = 0;
	for (int i = 0; i <= str.size(); i++) {

		// If we reached the end of the word or the end of the input.
		if (str[i] == separator || i == str.size()) {
			endIndex = i;
			string temp;
			temp.append(str, startIndex, endIndex - startIndex);
			formats.push_back(temp);
			startIndex = endIndex + 1;
		}
	}
}