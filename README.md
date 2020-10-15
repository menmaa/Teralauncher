# Teralauncher

A Dynamic-Link Library that provides functions to launch TERA and communicate with it.

## Exported Functions

```
void LaunchGame(LPCSTR lpSlsUrl, LPCSTR lpGameStr);
void RegisterMessageListener(void (*f)(LPCSTR, int));
void SendMessageToClient(LPCSTR responseTo, LPCSTR message);
```

## Example C# implementation

```
class TeraLauncher
{
	[DllImport("Teralauncher.dll", CallingConvention = CallingConvention.Cdecl)]
	public static extern void LaunchGame(string SlsUrl, string GameStr);

	[DllImport("Teralauncher.dll", CallingConvention = CallingConvention.Cdecl)]
	public static extern void RegisterMessageListener(IntPtr MethodPointer);

	[DllImport("Teralauncher.dll", CallingConvention = CallingConvention.Cdecl)]
	public static extern void SendMessageToClient(string responseTo, string Message);
}
```

## Register a Message Listener Method on C#

```
static class Program
{
	[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
	public delegate void MessageListenerDelegate(string message, int code);
	
	// We are defining these as statics so we always keep a reference to them to prevent the GC from clearing them.
	// If the GC clears them we'll receive an exception when native code attempts to call our Message Listener Method.
	static MessageListenerDelegate MyMessageListenerDelegate;
	static IntPtr MessageListenerPointer;

	static void Main()
	{
		MyMessageListenerDelegate = new MessageListenerDelegate(MessageListener);
		MessageListenerPointer = Marshal.GetFunctionPointerForDelegate(MyMessageListenerDelegate);
		TeraLauncher.RegisterMessageListener(MessageListenerPointer);
	}
	
	public static void MessageListener(string message, int code)
	{
		switch (message)
		{
			case "ticket":
				SendMessageToClient("ticket", "...");
				break;
			case "last_svr":
				SendMessageToClient("last_svr", "...");
				break;
			case "char_cnt":
				SendMessageToClient("char_cnt", "...");
				break;
			case "gameEvent":
				int gameEventCode = code;
				/// Handle game event messages here...
				break;
			case "endPopup":
				int endPopupCode = code;
				/// Handle end messages and popups here...
				break;
		}
	}
}
```