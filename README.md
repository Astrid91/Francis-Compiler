# FinalProj Compiler（中間碼 / 三位址碼產生器）

這是一個以 C++ 撰寫的簡易「兩階段（two-pass）」編譯器/翻譯器雛形。程式會讀取輸入檔中的類 Fortran/教學用語法（如 `PROGRAM / VARIABLE / DIMENSION / SUBROUTINE / CALL / IF / GTO / LABEL / ENP / ENS` 等），建立符號表，並將敘述轉換成「中間碼（intermediate code）」與對應的四元式/欄位表示，最後輸出成 `out_<input檔名>`。

> 注意：此專案依賴外部表格檔（例如 `Table1.table`、`Table2.table`…），並且目前程式內部將輸入檔名寫死為 `input1.txt`。

---

## 功能概要

- 讀取文字檔程式（預設 `input1.txt`）
- Pass 1：
  - 掃描指令與宣告
  - 建立/更新符號表（Identifier Table）
  - 針對 assignment / expression 產生三位址碼（3-address code）與暫存器 `T0, T1...`
  - 支援一維/二維陣列索引的展開（部分）
  - 產生初步輸出節點 `DATA`
- Pass 2：
  - 回填某些在 Pass 1 無法確定的資訊（例如 `CALL` 的符號查找）
- 輸出：
  - 以「行號 + (one,two,three,four) + interm」格式寫入 `out_input1.txt`（或其他輸入檔名）

---

## 專案結構與重要資料結構

### 主要類別
- `class FinalProj`
  - 封裝整個編譯流程：`readFile()` → `pass1()` → `pass2()` → `writeFile()`

### DATA（輸出節點）
`struct DATA` 會被存入 `vector<DATA> output`，代表一行中間碼/指令資訊：

- `line`：輸出序號
- `one/two/three/four`：四個欄位（類似四元式/指令欄位）
  - 每個欄位是 `(table_id, index)` 的概念
- `interm[40]`：可讀的中間碼字串
- `done`：是否已完成回填/處理

### Symbol / Table（部分）
- `Table0`: `T[]`（暫存器使用狀態）
- `Table3`: `Integer_Table`
- `Table4`: `RealNumber_Table`
- `Table5`: `Identifier_Table`
- `Table7`: `Information_Table`

---

## 依賴檔案（必備）

程式會在執行時讀取下列表格檔（位於執行目錄）：

- `Table1.table`：指令/運算子對照表（用於 `checkTable(str, "1")`）
- `Table2.table`：關係運算子/條件判斷對照表（用於 `checkTable(str, "2")`）
- （其他 table 檔在程式中也有結構，但是否使用取決於你的資料與流程）

> 若缺少 `Table1.table`、`Table2.table`，某些 `findElement()` 會找不到對應值，導致輸出欄位不完整或邏輯錯誤。

---

## 編譯與執行方式

### 編譯
```bash
g++ -std=c++11 -O2 -o finalproj main.cpp
```
### 執行
```bash
./finalproj
```
執行後會：
1. 讀入 `input1.txt`（目前程式寫死）
2. 產生輸出檔：`out_input1.txt`

---

## 輸入格式（概念）

此程式以「一行一敘述」為主要掃描方式，並且多數敘述必須以 `;` 結尾。
支援/辨識的關鍵字（依 `pass1()` 解析邏輯）包含：

- `PROGRAM <name>;`
- `VARIABLE : <A,B,C...>;`
- `DIMENSION : A( ... ), B( ... );`
- `SUBROUTINE <name>(...) : <args...>;`
- `CALL <name>(...);`
- `LABEL <L1,L2,...>;`
- `GTO <label>;`
- `IF <id> <op> <id> ...`
- `ENP;、ENS;`
- 以及 assignment/expression（例如 `A = B + 3;`）

---

## 輸出格式

輸出檔 `out_<input檔名>` 每一行大致長這樣：
```scheme
<line>  ( (t1,i1), (t2,i2), (t3,i3), (t4,i4) )    <intermediate string>
```
其中 `(t,i)` 代表「表格代號 + 表內索引」，例如：
`t=5` 可能代表 Identifier Table（Table5）
`t=3` 可能代表 Integer Table（Table3）或數字常數（依程式目前寫法）
`t=0` 代表暫存器 T[]

---

## 程式流程說明（重要函式）

- `compiler()`
  - 主流程：讀檔、初始化、Pass1、Pass2、寫檔

- `readFile()`
  - 讀取輸入檔內容到 `vector<string> input`

- `initializeTable5()`
  - 初始化 Identifier Table 的 `loc`

- `pass1()`
  - 逐行掃描、辨識關鍵字、建立符號表
  - 遇到 assignment/expression 時呼叫 `threeAddress()`

- `threeAddress(idx, j)`
  - 使用 `Oprand` 與 `Operator` 兩個 stack 做 expression parsing
  - 適時呼叫 `machineCode()` 產生中間碼
  - 嘗試處理：
    - 一般運算：`+ - * / ^`
    - 括號
    - 一維/二維陣列索引（部分）

- `machineCode(...)`
  - 實際生成 `DATA node`
  - 決定 operand 是：
    - 常數
    - 識別字
    - 暫存器 `T`
  - 產生對應的 `node.one/two/three/four` 與 `node.interm`

- `pass2()`
  - 回填 `CALL` 類指令中 subroutine 的 table index

- `writeFile()`
  - 將 `output` 寫到 `out_<fileName>`
