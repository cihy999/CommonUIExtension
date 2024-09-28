# CommonUIExtension
This is a plugin that extends the functionality of the Unreal Engine Plugin - Common UI.

Supports keyboard navigation functionality.

1. 關閉合成游標功能
    1. 它無法支援鍵盤
    2. 它在重疊的介面上，會有誤觸Hover的問題，比如：當選擇”否”關閉詢問視窗後，莫名選到跟”否”按鈕重疊位置的按鈕
2. 區分出滑鼠、鍵盤，只有鍵盤會表現得跟搖桿一樣
3. 鍵盤、搖桿仰賴Focus做出Hover
    1. FocusReceived、FocusLost觸發
    2. 收到Focus事件時，模擬滑鼠輸入事件去觸發相關處理
