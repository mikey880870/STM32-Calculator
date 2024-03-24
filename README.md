# STM32 Calculator
 搭配7段顯示器(MAX7219)及Keypad實現簡單計算機功能
 程式功能:
 1. 加減乘除計算
 2. 限制輸入最大三位
 3. 輸入第二個數字時若五秒內沒按下一位，就會進行計算
 4. Keypad 的11:'+'、12:'-'、13:'*'、14:'/'，15 為清除

 程式介紹:
 1.Keypad 的行透過 Systick 中斷，輪流切換每一個 COLUMN 的 OTYPER，Open Drain 為不讀取輸入；Push Pull 讀取。
 2.Keypad 的列透則過外部中斷，若按下去這列的行也讀取輸入，則代表 Keypad 有輸入。
 3.限定輸入3位數，會即時輸出數字至Max7219，若按了運算子且數字2不為零，則TIM3啟動倒數5秒，時間到就會進行運算。
 
 MAX7219 如下
 
![images](https://github.com/mikey880870/STM32-Calculator/assets/127172104/d04f8d1f-00e6-4ccb-8318-aba00b923087)

 Keypad 如下
 
![下載](https://github.com/mikey880870/STM32-Calculator/assets/127172104/20033779-1c8c-4e5e-8a17-77eb78ad1c17)
