package go_librespot

import (
	"runtime"
	"os"

	spotifypb "github.com/devgianlu/go-librespot/proto/spotify"
	clienttokenpb "github.com/devgianlu/go-librespot/proto/spotify/clienttoken/data/v0"
)

var GOOS string = runtime.GOOS;
var GOARCH string = runtime.GOARCH;

func GetOS() spotifypb.Os {
	if os.Getenv("GO_LIBRESPOT_OS") != ""{
		GOOS = os.Getenv("GO_LIBRESPOT_OS")
	}
	switch GOOS {
	case "android":
		return spotifypb.Os_OS_ANDROID
	case "darwin":
		return spotifypb.Os_OS_OSX
	case "freebsd":
		return spotifypb.Os_OS_FREEBSD
	case "ios":
		return spotifypb.Os_OS_IPHONE
	case "linux":
		return spotifypb.Os_OS_LINUX
	case "windows":
		return spotifypb.Os_OS_WINDOWS
	default:
		return spotifypb.Os_OS_UNKNOWN
	}
}

func GetCpuFamily() spotifypb.CpuFamily {
	if os.Getenv("GO_LIBRESPOT_ARCH") != ""{
		GOARCH = os.Getenv("GO_LIBRESPOT_ARCH")
	}
	switch GOARCH {
	case "386":
		return spotifypb.CpuFamily_CPU_X86
	case "amd64":
		return spotifypb.CpuFamily_CPU_X86_64
	case "arm":
		return spotifypb.CpuFamily_CPU_ARM
	case "arm64":
		return spotifypb.CpuFamily_CPU_ARM
	case "aarch64":
		return spotifypb.CpuFamily_CPU_ARM
	case "mips":
		return spotifypb.CpuFamily_CPU_MIPS
	case "mips64":
		return spotifypb.CpuFamily_CPU_MIPS
	case "ppc64":
		return spotifypb.CpuFamily_CPU_PPC_64
	default:
		return spotifypb.CpuFamily_CPU_UNKNOWN
	}
}

func GetPlatform() spotifypb.Platform {
	if os.Getenv("GO_LIBRESPOT_OS") != ""{
		GOOS = os.Getenv("GO_LIBRESPOT_OS")
	}
	if os.Getenv("GO_LIBRESPOT_ARCH") != ""{
		GOARCH = os.Getenv("GO_LIBRESPOT_ARCH")
	}
	switch GOOS {
	case "android":
		return spotifypb.Platform_PLATFORM_ANDROID_ARM
	case "darwin":
		switch GOARCH {
		case "386":
			return spotifypb.Platform_PLATFORM_OSX_X86
		case "amd64":
			return spotifypb.Platform_PLATFORM_OSX_X86_64
		case "ppc64":
			return spotifypb.Platform_PLATFORM_OSX_PPC
		}
	case "freebsd":
		switch GOARCH {
		case "386":
			return spotifypb.Platform_PLATFORM_FREEBSD_X86
		case "amd64":
			return spotifypb.Platform_PLATFORM_FREEBSD_X86_64
		}
	case "ios":
		switch GOARCH {
		case "arm":
			return spotifypb.Platform_PLATFORM_IPHONE_ARM
		case "arm64":
			return spotifypb.Platform_PLATFORM_IPHONE_ARM64
		}
	case "linux":
		switch GOARCH {
		case "386":
			return spotifypb.Platform_PLATFORM_LINUX_X86
		case "amd64":
			return spotifypb.Platform_PLATFORM_LINUX_X86_64
		case "mips":
			return spotifypb.Platform_PLATFORM_LINUX_MIPS
		case "mips64":
			return spotifypb.Platform_PLATFORM_LINUX_MIPS
		case "arm":
			return spotifypb.Platform_PLATFORM_LINUX_ARM
		case "arm64":
			return spotifypb.Platform_PLATFORM_LINUX_ARM
		case "aarch64":
			return spotifypb.Platform_PLATFORM_LINUX_ARM
		}
	case "windows":
		switch GOARCH {
		case "386":
			return spotifypb.Platform_PLATFORM_WIN32_X86
		case "amd64":
			return spotifypb.Platform_PLATFORM_WIN32_X86_64
		case "arm":
			return spotifypb.Platform_PLATFORM_WINDOWS_CE_ARM
		case "arm64":
			return spotifypb.Platform_PLATFORM_WINDOWS_CE_ARM
		}
	case "js":
		return spotifypb.Platform_PLATFORM_WEBPLAYER
	}

	return spotifypb.Platform_PLATFORM_GENERIC_PARTNER
}

func GetPlatformSpecificData() *clienttokenpb.PlatformSpecificData {
	if os.Getenv("GO_LIBRESPOT_OS") != ""{
		GOOS = os.Getenv("GO_LIBRESPOT_OS")
	}
	switch GOOS {
	case "android":
		return &clienttokenpb.PlatformSpecificData{
			Data: &clienttokenpb.PlatformSpecificData_Android{
				Android: &clienttokenpb.NativeAndroidData{},
			},
		}
	case "darwin":
		return &clienttokenpb.PlatformSpecificData{
			Data: &clienttokenpb.PlatformSpecificData_DesktopMacos{
				DesktopMacos: &clienttokenpb.NativeDesktopMacOSData{},
			},
		}
	case "ios":
		return &clienttokenpb.PlatformSpecificData{
			Data: &clienttokenpb.PlatformSpecificData_Ios{
				Ios: &clienttokenpb.NativeIOSData{},
			},
		}
	case "linux":
		return &clienttokenpb.PlatformSpecificData{
			Data: &clienttokenpb.PlatformSpecificData_DesktopLinux{
				DesktopLinux: &clienttokenpb.NativeDesktopLinuxData{},
			},
		}
	case "windows":
		return &clienttokenpb.PlatformSpecificData{
			Data: &clienttokenpb.PlatformSpecificData_DesktopWindows{
				DesktopWindows: &clienttokenpb.NativeDesktopWindowsData{},
			},
		}
	}
	return nil
}
