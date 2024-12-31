#pragma once

#include "MainWindow.g.h"

//6-2で追加。OpenCVのヘッダーをインクルード
#include <opencv2/opencv.hpp>

namespace winrt::WinUI_OpenCV::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow()
        {
            //6-1で追加。タイトル変更
			Title(L"WinUI_OpenCV");
        }

        //6-1で追加。ウインドウハンドルの取得
        HWND GetHWND();

		//6-2で追加。wstringからstringへの変換
		std::string WStringToString(const std::wstring& utf16);
		//6-2で追加。stringからwstringへの変換
		std::wstring StringToWString(const std::string& jisstr);

		//6-3で追加。MatからBitmapImageへの変換
        winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage> MatToBitmapAsync(const cv::Mat& mat);

		//6-3で追加。ウインドウサイズを画像サイズに合わせる
        void ResizeWindowToImage(const winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage& bitmap);

        //6-4で追加。画像サイズを変更
        void ResizeImage(const double scale);

        //イベントハンドラ
        
		//6-1で変更。非同期化
        winrt::Windows::Foundation::IAsyncAction Open_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        void Exit_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        //6-4で変更。非同期化
        winrt::Windows::Foundation::IAsyncAction Enlarge2Times_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        //6-4で変更。非同期化
        winrt::Windows::Foundation::IAsyncAction Reduce2Times_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

	private:
		//6-2で追加。画像データ保存メンバ
        cv::Mat _loadImg;

    };
}

namespace winrt::WinUI_OpenCV::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}

/**
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**/
