#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

//6-1で追加。::IWindowNativeと::IInitializeWithWindowを使うのにmicrosoft.ui.xaml.window.h、Shobjidl.hが必要
#include <microsoft.ui.xaml.window.h>
#include <Shobjidl.h>

//6-1で追加。FilePickerを使用するのにwinrt/Windows.Storage.Pickers.hが必要
#include <winrt/Windows.Storage.Pickers.h>

//6-3で追加。InMemoryRandomAccessStreamを使用するのにwinrt/Windows.Storage.Streams.hが必要
#include <winrt/Windows.Storage.Streams.h>

//6-3で追加。BitmapImageを使用するのにwinrt/Microsoft.UI.Xaml.Media.Imaging.hが必要
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

//6-3で追加GetWindowIdFromWindowを使用するのにwinrt/Microsoft.ui.interop.hとwinrt/Microsoft.UI.Windowing.hが必要
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Windowing.h>

//6-3で追加。Windows.Graphics.SizeInt32を使用するのにwinrt/Windows.Graphics.hが必要
#include <winrt/Windows.Gaming.Input.h>


namespace winrt::WinUI_OpenCV::implementation
{

}

//6-1で追加。ウインドウハンドルの取得
HWND winrt::WinUI_OpenCV::implementation::MainWindow::GetHWND()
{
	auto windowNative{ this->m_inner.as<::IWindowNative>() };
	HWND hWnd{ 0 };
	windowNative->get_WindowHandle(&hWnd);

	return hWnd;
}

//6-2で追加。wstringからstringへの変換
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

//6-2で追加。stringからwstringへの変換
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

//6-3で追加。MatからBitmapImageへの変換
winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage> winrt::WinUI_OpenCV::implementation::MainWindow::MatToBitmapAsync(const cv::Mat& mat)
{
	if (mat.empty())
		co_return nullptr;

	//Matから変換されるバイト配列格納用
	std::vector<uchar> imagebuf;

	//①imencodeを使用してバイト配列へエンコード。今回はbmp形式
	cv::imencode(".bmp", mat, imagebuf);

	//②InMemoryRandomAccessStreamを作成
	winrt::Windows::Storage::Streams::InMemoryRandomAccessStream rcstream;

	//③②のInMemoryRandomAccessStreamに関連付けたIOutputStreamを作成
	auto outputstream = rcstream.GetOutputStreamAt(0);

	//④③のIOutputStreamに関連付けたDataWriterを作成
	winrt::Windows::Storage::Streams::DataWriter writer(outputstream);

	//⑤④のDataWriterを使用してストリームへデータを書き込む
	writer.WriteBytes(winrt::array_view<const uint8_t>(imagebuf.data(), imagebuf.data() + imagebuf.size()));

	//書き込みを確定&ストリームをフラッシュ
	co_await writer.StoreAsync();
	co_await outputstream.FlushAsync();

	//⑥②で作成したRandomAccessStreamの位置を0へ戻す
	rcstream.Seek(0);

	//⑦SetSourceAsyncでBitmapへ変換
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
	co_await bitmap.SetSourceAsync(rcstream);

	co_return bitmap;
}

//6-3で追加。ウインドウサイズを画像サイズに合わせる
void winrt::WinUI_OpenCV::implementation::MainWindow::ResizeWindowToImage(const winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage& bitmap)
{
	//①ウインドウIDからAppWindowを作成。
	auto wndid = Microsoft::UI::GetWindowIdFromWindow(GetHWND());
	auto appwnd = Microsoft::UI::Windowing::AppWindow::GetFromWindowId(wndid);

	//②Bitmapのサイズを取得＋高さにはmenuの高さ
	auto menuheight = _menuBar().ActualHeight();
	winrt::Windows::Graphics::SizeInt32 imgsize;
	imgsize.Width = bitmap.PixelWidth();
	imgsize.Height = bitmap.PixelHeight() + menuheight;

	//③AppWindowのResize()を使用してウインドウサイズを変更
	appwnd.Resize(imgsize);
}

void winrt::WinUI_OpenCV::implementation::MainWindow::ResizeImage(const double scale)
{
	if (_loadImg.empty())
		return;

	//画像を拡大縮小. cv::resize()を使用
	cv::Mat resizedImg;
	cv::resize(_loadImg, resizedImg, cv::Size(), scale, scale, cv::INTER_CUBIC);

	//_loadImgをresizedImgで上書き
	_loadImg = resizedImg.clone();
}

winrt::Windows::Foundation::IAsyncAction winrt::WinUI_OpenCV::implementation::MainWindow::Open_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	//6-1で追加。FileOpenPickerを使用してファイルを開く
	winrt::Windows::Storage::Pickers::FileOpenPicker imgpicker;
	imgpicker.as<::IInitializeWithWindow>()->Initialize(this->GetHWND());
	imgpicker.FileTypeFilter().Append(L".jpg");
	imgpicker.FileTypeFilter().Append(L".png");
	imgpicker.FileTypeFilter().Append(L".bmp");
	imgpicker.FileTypeFilter().Append(L".gif");
	imgpicker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::Desktop);

	auto&& imgfile = co_await imgpicker.PickSingleFileAsync();

	//6-1で追加。ファイルが選択されなかった場合はco_returnで関数を終了
	if (!imgfile)
	{
		co_return;
	}

	//6-1で追加。ファイルパスの取得
	std::wstring imgpath = imgfile.Path().c_str();

	//6-2で追加。wstringからstringへの変換
	std::string utf8path = WStringToString(imgpath);

	//6-2で追加。OpenCVで画像を読み込む
	_loadImg = cv::imread(utf8path);

	//6-2で追加。画像データを表示 6-4でコメントアウト
	//cv::imshow("Load Image", _loadImg);

	//6-3で追加。MatからBitmapImageへ変換
	auto bitmap = co_await MatToBitmapAsync(_loadImg);

	//6-3で追加。ウインドウサイズを画像サイズに合わせる
	ResizeWindowToImage(bitmap);

	//6-3で追加。画像を表示
	_showImage().Source(bitmap);

}

void winrt::WinUI_OpenCV::implementation::MainWindow::Exit_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	//6-1で追加。アプリの終了
	Application::Current().Exit();
}

//6-4で変更。非同期化
winrt::Windows::Foundation::IAsyncAction winrt::WinUI_OpenCV::implementation::MainWindow::Enlarge2Times_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	//面積2倍なので1辺√2倍
	ResizeImage(std::sqrt(2));

	auto&& bitmap = co_await MatToBitmapAsync(_loadImg);

	ResizeWindowToImage(bitmap);
	_showImage().Source(bitmap);
}

//6-4で変更。非同期化
winrt::Windows::Foundation::IAsyncAction winrt::WinUI_OpenCV::implementation::MainWindow::Reduce2Times_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	//面積1/2倍なので1辺√1/2倍
	ResizeImage(1 / std::sqrt(2));

	auto&& bitmap = co_await MatToBitmapAsync(_loadImg);

	ResizeWindowToImage(bitmap);
	_showImage().Source(bitmap);
}
