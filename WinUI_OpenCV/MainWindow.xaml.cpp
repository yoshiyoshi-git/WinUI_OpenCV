#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

//6-1�Œǉ��B::IWindowNative��::IInitializeWithWindow���g���̂�microsoft.ui.xaml.window.h�AShobjidl.h���K�v
#include <microsoft.ui.xaml.window.h>
#include <Shobjidl.h>

//6-1�Œǉ��BFilePicker���g�p����̂�winrt/Windows.Storage.Pickers.h���K�v
#include <winrt/Windows.Storage.Pickers.h>

//6-3�Œǉ��BInMemoryRandomAccessStream���g�p����̂�winrt/Windows.Storage.Streams.h���K�v
#include <winrt/Windows.Storage.Streams.h>

//6-3�Œǉ��BBitmapImage���g�p����̂�winrt/Microsoft.UI.Xaml.Media.Imaging.h���K�v
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

//6-3�Œǉ�GetWindowIdFromWindow���g�p����̂�winrt/Microsoft.ui.interop.h��winrt/Microsoft.UI.Windowing.h���K�v
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Windowing.h>

//6-3�Œǉ��BWindows.Graphics.SizeInt32���g�p����̂�winrt/Windows.Graphics.h���K�v
#include <winrt/Windows.Gaming.Input.h>


namespace winrt::WinUI_OpenCV::implementation
{

}

//6-1�Œǉ��B�E�C���h�E�n���h���̎擾
HWND winrt::WinUI_OpenCV::implementation::MainWindow::GetHWND()
{
	auto windowNative{ this->m_inner.as<::IWindowNative>() };
	HWND hWnd{ 0 };
	windowNative->get_WindowHandle(&hWnd);

	return hWnd;
}

//6-2�Œǉ��Bwstring����string�ւ̕ϊ�
std::string winrt::WinUI_OpenCV::implementation::MainWindow::WStringToString(const std::wstring& utf16)
{
	std::string jisstr;
	int jissize = WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (jissize > 0)
	{
		jisstr.resize(jissize);
		WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), -1, jisstr.data(), jissize, nullptr, nullptr);
	}
	return jisstr;
}

//6-2�Œǉ��Bstring����wstring�ւ̕ϊ�
std::wstring winrt::WinUI_OpenCV::implementation::MainWindow::StringToWString(const std::string& jisstr)
{
	std::wstring utf16;
	int utf16size = MultiByteToWideChar(CP_ACP, 0, jisstr.c_str(), -1, nullptr, 0);
	if (utf16size > 0)
	{
		utf16.resize(utf16size);
		MultiByteToWideChar(CP_ACP, 0, jisstr.c_str(), -1, utf16.data(), utf16size);
	}
	return utf16;	
}

//6-3�Œǉ��BMat����BitmapImage�ւ̕ϊ�
winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage> winrt::WinUI_OpenCV::implementation::MainWindow::MatToBitmapAsync(const cv::Mat& mat)
{
	if (mat.empty())
		co_return nullptr;

	//Mat����ϊ������o�C�g�z��i�[�p
	std::vector<uchar> imagebuf;

	//�@imencode���g�p���ăo�C�g�z��փG���R�[�h�B�����bmp�`��
	cv::imencode(".bmp", mat, imagebuf);

	//�AInMemoryRandomAccessStream���쐬
	winrt::Windows::Storage::Streams::InMemoryRandomAccessStream rcstream;

	//�B�A��InMemoryRandomAccessStream�Ɋ֘A�t����IOutputStream���쐬
	auto outputstream = rcstream.GetOutputStreamAt(0);

	//�C�B��IOutputStream�Ɋ֘A�t����DataWriter���쐬
	winrt::Windows::Storage::Streams::DataWriter writer(outputstream);

	//�D�C��DataWriter���g�p���ăX�g���[���փf�[�^����������
	writer.WriteBytes(winrt::array_view<const uint8_t>(imagebuf.data(), imagebuf.data() + imagebuf.size()));

	//�������݂��m��&�X�g���[�����t���b�V��
	co_await writer.StoreAsync();
	co_await outputstream.FlushAsync();

	//�E�A�ō쐬����RandomAccessStream�̈ʒu��0�֖߂�
	rcstream.Seek(0);

	//�FSetSourceAsync��Bitmap�֕ϊ�
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
	co_await bitmap.SetSourceAsync(rcstream);

	co_return bitmap;
}

//6-3�Œǉ��B�E�C���h�E�T�C�Y���摜�T�C�Y�ɍ��킹��
void winrt::WinUI_OpenCV::implementation::MainWindow::ResizeWindowToImage(const winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage& bitmap)
{
	//�@�E�C���h�EID����AppWindow���쐬�B
	auto wndid = Microsoft::UI::GetWindowIdFromWindow(GetHWND());
	auto appwnd = Microsoft::UI::Windowing::AppWindow::GetFromWindowId(wndid);

	//�ABitmap�̃T�C�Y���擾�{�����ɂ�menu�̍���
	auto menuheight = _menuBar().ActualHeight();
	winrt::Windows::Graphics::SizeInt32 imgsize;
	imgsize.Width = bitmap.PixelWidth();
	imgsize.Height = bitmap.PixelHeight() + menuheight;

	//�BAppWindow��Resize()���g�p���ăE�C���h�E�T�C�Y��ύX
	appwnd.Resize(imgsize);
}

void winrt::WinUI_OpenCV::implementation::MainWindow::ResizeImage(const double scale)
{
	if (_loadImg.empty())
		return;

	//�摜���g��k��. cv::resize()���g�p
	cv::Mat resizedImg;
	cv::resize(_loadImg, resizedImg, cv::Size(), scale, scale, cv::INTER_CUBIC);

	//_loadImg��resizedImg�ŏ㏑��
	_loadImg = resizedImg.clone();
}

winrt::Windows::Foundation::IAsyncAction winrt::WinUI_OpenCV::implementation::MainWindow::Open_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	//6-1�Œǉ��BFileOpenPicker���g�p���ăt�@�C�����J��
	winrt::Windows::Storage::Pickers::FileOpenPicker imgpicker;
	imgpicker.as<::IInitializeWithWindow>()->Initialize(this->GetHWND());
	imgpicker.FileTypeFilter().Append(L".jpg");
	imgpicker.FileTypeFilter().Append(L".png");
	imgpicker.FileTypeFilter().Append(L".bmp");
	imgpicker.FileTypeFilter().Append(L".gif");
	imgpicker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::Desktop);

	auto&& imgfile = co_await imgpicker.PickSingleFileAsync();

	//6-1�Œǉ��B�t�@�C�����I������Ȃ������ꍇ��co_return�Ŋ֐����I��
	if (!imgfile)
	{
		co_return;
	}

	//6-1�Œǉ��B�t�@�C���p�X�̎擾
	std::wstring imgpath = imgfile.Path().c_str();

	//6-2�Œǉ��Bwstring����string�ւ̕ϊ�
	std::string utf8path = WStringToString(imgpath);

	//6-2�Œǉ��BOpenCV�ŉ摜��ǂݍ���
	_loadImg = cv::imread(utf8path);

	//6-2�Œǉ��B�摜�f�[�^��\�� 6-4�ŃR�����g�A�E�g
	//cv::imshow("Load Image", _loadImg);

	//6-3�Œǉ��BMat����BitmapImage�֕ϊ�
	auto bitmap = co_await MatToBitmapAsync(_loadImg);

	//6-3�Œǉ��B�E�C���h�E�T�C�Y���摜�T�C�Y�ɍ��킹��
	ResizeWindowToImage(bitmap);

	//6-3�Œǉ��B�摜��\��
	_showImage().Source(bitmap);

}

void winrt::WinUI_OpenCV::implementation::MainWindow::Exit_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	//6-1�Œǉ��B�A�v���̏I��
	Application::Current().Exit();
}

//6-4�ŕύX�B�񓯊���
winrt::Windows::Foundation::IAsyncAction winrt::WinUI_OpenCV::implementation::MainWindow::Enlarge2Times_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	//�ʐ�2�{�Ȃ̂�1�Ӂ�2�{
	ResizeImage(std::sqrt(2));

	auto&& bitmap = co_await MatToBitmapAsync(_loadImg);

	ResizeWindowToImage(bitmap);
	_showImage().Source(bitmap);
}

//6-4�ŕύX�B�񓯊���
winrt::Windows::Foundation::IAsyncAction winrt::WinUI_OpenCV::implementation::MainWindow::Reduce2Times_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	//�ʐ�1/2�{�Ȃ̂�1�Ӂ�1/2�{
	ResizeImage(1 / std::sqrt(2));

	auto&& bitmap = co_await MatToBitmapAsync(_loadImg);

	ResizeWindowToImage(bitmap);
	_showImage().Source(bitmap);
}
