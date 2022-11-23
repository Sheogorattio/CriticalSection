#include "CriticalSectionDlg.h"
#include <tchar.h>

CriticalSectionDlg* CriticalSectionDlg::ptr = NULL;

CRITICAL_SECTION cs;

INT N = 0;
TCHAR szFileName[50]={};

CriticalSectionDlg::CriticalSectionDlg(void)
{
	srand(time(0));
	N = rand() % (5 - 1) + 1;
	ptr = this;
}

CriticalSectionDlg::~CriticalSectionDlg(void)
{
	DeleteCriticalSection(&cs);
}

void CriticalSectionDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL CriticalSectionDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	InitializeCriticalSection(&cs);
	return TRUE;
}

DWORD WINAPI Write(LPVOID lp)
{
	srand(time(0));
	EnterCriticalSection(&cs);
	for (int i = 0; i < N; i++) {
		_stprintf_s(szFileName, TEXT("Array(%d).txt"), i+1);
		ofstream out(szFileName, ios::out | ios::trunc);
		if (!out.is_open())
		{
			MessageBox(0, TEXT("Error"), TEXT("Критическая секция"), MB_OK);
			return 1;
		}
		int A[100];
		for (int i = 0; i < 100; i++)
		{
			A[i] = rand() % 50;
			out << A[i] << ' ';
		}
		out.close();
	}
	LeaveCriticalSection(&cs);
	MessageBox(0, TEXT("Поток записал информацию в файл"), TEXT("Критическая секция"), MB_OK);
	return 0;
}

DWORD WINAPI Read(LPVOID lp)
{
	EnterCriticalSection(&cs);
	int B[100];
	int sum = 0;
	ofstream out(TEXT("array.txt"), ios::trunc | ios:: out);
	
	for (int i = 0; i < N; i++) {
		
		_stprintf_s(szFileName, TEXT("Array(%d).txt"), i+1);
		ifstream tempIn(szFileName, ios::in);

		if (!out.is_open() && !tempIn.is_open())
		{
			MessageBox(0, TEXT("Error"), TEXT("Error"), MB_OK);
			return TRUE;
		}
		
		for (int i = 0; i < 100; i++)
		{
			tempIn >> B[i];
			out << B[i] << ' ';
			sum += B[i];
		}
		
		out << endl;

		tempIn.close();
	}
	out.close();
	LeaveCriticalSection(&cs);
	MessageBox(0, TEXT("Поток прочитал информацию из файла"), TEXT("Критическая секция"), MB_OK);
	TCHAR str[30];
	wsprintf(str, TEXT("Сумма чисел равна %d"), sum);
	MessageBox(0, str, TEXT("Критическая секция"), MB_OK);
	return 0;
}


void CriticalSectionDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if(id == IDC_BUTTON1)
	{
		HANDLE hThread = CreateThread(NULL, 0, Write, 0, 0, NULL); 
		CloseHandle(hThread);
		hThread = CreateThread(NULL, 0, Read, 0, 0, NULL);  
		CloseHandle(hThread);
	}
}

BOOL CALLBACK CriticalSectionDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}