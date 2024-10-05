# CommonUIExtension
This is a plugin that extends the functionality of the Unreal Engine Plugin - Common UI.

Supports keyboard navigation functionality.

Unreal Engine: 5.4

Common UI本身不支援鍵盤操作，網路的解決方案：

- [Common UI Plugin - Keyboard Navigation](https://dev.epicgames.com/community/learning/tutorials/B5R6/unreal-engine-common-ui-plugin-keyboard-navigation)
- [UE5 - Common UI: Keyboard Controls Setup](https://youtu.be/aDoNLyUVy2o?si=tlOQ9Xv8JRFqyCm2)

感覺還不夠好，但有提供一些方向，再加上自己的想法：

1. 關閉合成游標功能
   - 它無法支援鍵盤
   - 它在重疊的介面上，會有誤觸Hover的問題，比如：當選擇”否”關閉詢問視窗後，莫名選到跟”否”按鈕重疊位置的按鈕
2. 鍵盤、搖桿仰賴Focus做出Hover
   於`OnFocusReceived`、`OnFocusLost`觸發，當收到Focus事件，模擬滑鼠輸入事件去觸發相關處理
3. 區分出滑鼠、鍵盤，只有鍵盤會表現得跟搖桿一樣
   - 需要考慮到`SetInteractionEnabled`相關設置，當輸入改變時，直接模擬滑鼠輸入事件是不容易出錯的方法
   - 需要紀錄Focus狀態，才能判斷`bIsHoveredNow = bIsFocused && bInIsHoverEnabledOnFocus`
    - `bIsHoveredNow`→ `OnMouseEnter`
    - `!bIsHoveredNow`→ `OnMouseLeave`

筆記：[Common UI - 支援Keyboard](https://hackmd.io/@Ejvlpuy-SD2K0IjO3REe0g/S1rgYVA0C)
