#include "Library.h"
using namespace std;

static TCHAR szWindowClass[] = _T("DocFlowApp");
static TCHAR szTitle[] = _T("Document Flow");

//window size
unsigned short widthWnd = 1280;
unsigned short heightWnd = 720;

HINSTANCE hInst;

HWND editDocument;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

namespace fs = std::filesystem;
using directory_iterator = std::filesystem::directory_iterator;
std::string path;

HTREEITEM AddItemtotree(HWND hwndTV, LPWSTR LPSZITEM, HTREEITEM hParent);
BOOL InitTreeViewItems(HWND HWNDTV);
HWND CreateATreeView(HWND hwndParent);
bool dirExists(const std::string& dirName_in);
void CreateFileM(string filepath);
void ReadFile(string path);

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
	wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
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

	HWND openFileB = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"O",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // ����� 
		42,         // x position (������� �)
		10,         // y position (������� �)
		32,        // Button width (������)
		32,        // Button height (������)
		hWnd,     // ������������ ���� (������� ����)
		(HMENU)1002,       //Menu.
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

	HWND addDocB = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"+",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // ����� 
		10,         // x position (������� �)
		70,         // y position (������� �)
		32,        // Button width (������)
		32,        // Button height (������)
		hWnd,     // ������������ ���� (������� ����)
		(HMENU)1007,       //Menu.
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

	HWND remDocB = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"-",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // ����� 
		42,         // x position (������� �)
		70,         // y position (������� �)
		32,        // Button width (������)
		32,        // Button height (������)
		hWnd,     // ������������ ���� (������� ����)
		(HMENU)1008,       //Menu.
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

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
	string filepath = path + "\\create.txt";
	LPNMHDR pHdr = reinterpret_cast<LPNMHDR>(lParam);
	NMTREEVIEW* pnmtv = (LPNMTREEVIEW)lParam;
	int dialog{};
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case 1010:
				//CreateFileM(filepath);
				//TreeView_DeleteAllItems(tv_parent); //������� ������
				//InitTreeViewItems(tv_parent); // ������������� ������
				dialog = DialogBox(hInst, MAKEINTRESOURCE(NewDocumentWindow), NULL, (DLGPROC)DialogProc);
				if (dialog == 0)
				{
					//���� ���� ������ ������ ������ ��� �������
					
				}
				else if (dialog == 1)
				{
					//���� ���� ������ ������ ��
					
				}
				return dialog;
			case 1002:
				ReadFile(filepath);
				break;
			case 1007:
				//AddItemtotree(tv_parent, const_cast<LPTSTR>(TEXT("D1")), NULL);
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
				AddItemtotree(tv_parent, const_cast<LPTSTR>(TEXT("D1")), NULL);
				break;
			case NM_DBLCLK:
				TCHAR buffer[32];
				TVITEM item;
				item.hItem = TreeView_GetSelection(tv_parent);
				item.mask = TVIF_TEXT | TVIF_PARAM;
				item.cchTextMax = 32;
				item.pszText = buffer;

				TreeView_GetItem(tv_parent, &item);
				
				wstring wpathname = wstring(item.pszText);
				string pathname = string(wpathname.begin(), wpathname.end());

				ReadFile(path+"\\"+pathname);
				//SetWindowText(editDocument, item.pszText);
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
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, 1);
			return TRUE;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	case WM_INITDIALOG:
		return FALSE;
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return FALSE;
	}

	return FALSE;
}

void CreateFileM(string filepath) {
	ofstream o(filepath);
}


HWND CreateATreeView(HWND hWnd)
{
	HWND hwndTV;
	hwndTV = CreateWindowEx(0,
		WC_TREEVIEW,
		TEXT("Tree View"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
		10,
		105,
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

void ReadFile(string path) {
	string line;
	string result;
	ifstream in(path); // �������� ���� ��� ������
	if (in.is_open())
	{
		while (getline(in, line))
		{
			result += line+"\r\n";
			
		}
		wstring wline = wstring(result.begin(), result.end());
		LPCWSTR cwline = wline.c_str();
		SetWindowText(editDocument, cwline);
	}
	in.close();     // ��������� ����
}