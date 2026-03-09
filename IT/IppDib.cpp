#include "pch.h"
#include "IppDib.h"

#define DIB_HEADER_MARKER ((WORD) ('M' << 8) | 'B')

BOOL IppDib::Load(const char* filename) {
	const char* ext = strrchr(filename, '.');
	if (!_strcmpi(ext, ".bmp"))
		return LoadBMP(filename);
	else
		return FALSE;
}

BOOL IppDib::Save(const char* filename) {
	const char* ext = strrchr(filename, '.');
	if (!_strcmpi(ext, ".bmp"))
		return SaveBMP(filename);
	else return FALSE;
}

BOOL IppDib::LoadBMP(const char* filename) {
	FILE* fp = NULL;
	fopen_s(&fp, filename, "rb");
	if (!fp)
		return false;

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

	if (fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp) != 1) {
		fclose(fp);
		return false;
	}

	if (bmfh.bfType != DIB_HEADER_MARKER) {
		fclose(fp);
		return false;
	}

	if (fread(&bmih, sizeof(BITMAPINFOHEADER), 1, fp) != 1) {
		fclose(fp);
		return false;
	}
	
	m_nWidth = bmih.biWidth;
	m_nHeight = bmih.biHeight;
	m_nBitCount = bmih.biBitCount;

	DWORD dwWidthStep = (DWORD)((m_nWidth * m_nBitCount / 8 + 3) & ~3);
	DWORD dwSizeImage = m_nHeight * dwWidthStep;

	if (m_nBitCount == 24)
		m_nDibSize = sizeof(BITMAPINFOHEADER) + dwSizeImage;
	else
		m_nDibSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << m_nBitCount) + dwSizeImage;

	if (m_pDib)
		DestroyBitmap();

	m_pDib = new BYTE[m_nDibSize];
	if (m_pDib == NULL) {
		fclose(fp);
		return FALSE;
	}

	fseek(fp, sizeof(BITMAPFILEHEADER), SEEK_SET);
	if (fread(m_pDib, sizeof(BYTE), m_nDibSize, fp) != m_nDibSize) {
		delete[] m_pDib;
		m_pDib = NULL;
		fclose(fp);
		return FALSE;
	}
	fclose(fp);

	return TRUE;
}

BOOL IppDib::SaveBMP(const char* filename) {
	if (!IsValid())
		return FALSE;

	FILE* fp;
	fopen_s(&fp, filename, "wb");
	if (!fp)
		return FALSE;

	BITMAPFILEHEADER bmfh;
	bmfh.bfType = DIB_HEADER_MARKER;
	bmfh.bfSize = (DWORD)sizeof(BITMAPFILEHEADER) + m_nDibSize;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = (DWORD)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * GetPaletteNums()));

	fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(GetBitmapInfoAddr(), m_nDibSize, 1, fp);

	fclose(fp);

	return TRUE;
}

IppDib::IppDib()
	: m_nWidth(0), m_nHeight(0), m_nBitCount(0), m_nDibSize(0), m_pDib(NULL)
{
}

IppDib::IppDib(const IppDib& dib)
	: m_nWidth(dib.m_nWidth), m_nHeight(dib.m_nHeight), m_nBitCount(dib.m_nBitCount),
	m_nDibSize(dib.m_nDibSize), m_pDib(NULL)
{
	if (dib.m_pDib != NULL) {
		m_pDib = new BYTE[m_nDibSize];
		memcpy(m_pDib, dib.m_pDib, m_nDibSize);
	}
}

IppDib::~IppDib() {
	if (m_pDib)
		delete[] m_pDib;
}

BOOL IppDib::CreateGrayBitmap(LONG nWidth, LONG nHeight) {
	if (m_pDib)
		DestroyBitmap();

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nBitCount = 8;

	DWORD dwWidthStep = (m_nWidth * m_nBitCount / 8 + 3) & ~3;
	DWORD dwSizeImage = (m_nHeight * dwWidthStep);
	m_nDibSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * GetPaletteNums()) + dwSizeImage;

	m_pDib = new BYTE[m_nDibSize];
	if (m_pDib == NULL)
		return FALSE;

	LPBITMAPINFOHEADER Ipbmi = (LPBITMAPINFOHEADER)m_pDib;
	Ipbmi->biSize = sizeof(BITMAPINFOHEADER);
	Ipbmi->biWidth = m_nWidth;
	Ipbmi->biHeight = m_nHeight;
	Ipbmi->biPlanes = 1;
	Ipbmi->biBitCount = m_nBitCount;
	Ipbmi->biCompression = BI_RGB;
	Ipbmi->biSizeImage = dwSizeImage;
	Ipbmi->biXPelsPerMeter = 0;
	Ipbmi->biYPelsPerMeter = 0;
	Ipbmi->biClrUsed = 0;
	Ipbmi->biClrImportant = 0;

	RGBQUAD* pPal = (RGBQUAD*)((BYTE*)m_pDib + sizeof(BITMAPINFOHEADER));
	for (int i = 0; i < 256; i++) {
		pPal->rgbBlue = (BYTE)i;
		pPal->rgbGreen = (BYTE)i;
		pPal->rgbRed = (BYTE)i;
		pPal->rgbReserved = 0;
		pPal++;
	}

	BYTE* pData = GetDIBitsAddr();
	memset(pData, 0, dwSizeImage);

	return TRUE;
}

BOOL IppDib::CreateRgbBitmap(LONG nWidth, LONG nHeight) {
	if (m_pDib)
		DestroyBitmap();

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nBitCount = 24;

	DWORD dwWidthStep = (m_nWidth * m_nBitCount / 8 + 3) & ~3;
	DWORD dwSizeImage = (m_nHeight * dwWidthStep);
	m_nDibSize = sizeof(BITMAPINFOHEADER) + dwSizeImage;

	m_pDib = new BYTE[m_nDibSize];
	if (m_pDib == NULL)
		return FALSE;

	LPBITMAPINFOHEADER Ipbmi = (LPBITMAPINFOHEADER)m_pDib;
	Ipbmi->biSize = sizeof(BITMAPINFOHEADER);
	Ipbmi->biWidth = m_nWidth;
	Ipbmi->biHeight = m_nHeight;
	Ipbmi->biPlanes = 1;
	Ipbmi->biBitCount = m_nBitCount;
	Ipbmi->biCompression = BI_RGB;
	Ipbmi->biSizeImage = dwSizeImage;
	Ipbmi->biXPelsPerMeter = 0;
	Ipbmi->biYPelsPerMeter = 0;
	Ipbmi->biClrUsed = 0;
	Ipbmi->biClrImportant = 0;
	
	BYTE* pData = GetDIBitsAddr();
	memset(pData, 0, dwSizeImage);

	return TRUE;
}
void IppDib::DestroyBitmap() {
	if (m_pDib) {
		delete[]m_pDib;
		m_pDib = NULL;
	}

	m_nWidth = 0;
	m_nHeight = 0;
	m_nBitCount = 0;
	m_nDibSize = 0;	
}
void IppDib::Draw(HDC hdc, int dx, int dy) {
	if (m_pDib == NULL)
		return;

	LPBITMAPINFO lpbi = (LPBITMAPINFO)m_pDib;
	LPVOID lpBits = (LPVOID)GetDIBitsAddr();

	::SetDIBitsToDevice(hdc,
		dx,
		dy,
		m_nWidth,
		m_nHeight,
		0,
		0,
		0,
		m_nHeight,
		lpBits,
		lpbi,
		DIB_RGB_COLORS);
}

void IppDib::Draw(HDC hdc, int dx, int dy, int dw, int dh, DWORD dwRop) {
	Draw(hdc, dx, dy, dw, dh, 0, 0, m_nWidth, m_nHeight, dwRop);
}

void IppDib::Draw(HDC hdc, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, DWORD dwRop) {
	if (m_pDib == NULL)
		return;

	LPBITMAPINFO lpbi = (LPBITMAPINFO)m_pDib;
	LPSTR lpDIBBits = (LPSTR)GetDIBitsAddr();

	SetStretchBltMode(hdc, COLORONCOLOR);
	::StretchDIBits(hdc,dx,
		dy,
		dw,
		dh,
		sx,
		sy,
		sw,
		sh,
		lpDIBBits,
		lpbi,
		DIB_RGB_COLORS,
		SRCCOPY);
}

BYTE* IppDib::GetDIBitsAddr() const {
	if (m_pDib == NULL)
		return NULL;

	LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER)m_pDib;
	return ((BYTE*)m_pDib + lpbmi->biSize + (sizeof(RGBQUAD) * GetPaletteNums()));
}

int IppDib::GetPaletteNums() const
{
	switch (m_nBitCount) {
	case 1: return 2;
	case 4: return 16;
	case 8: return 256;
	default: return 0;
	}
}