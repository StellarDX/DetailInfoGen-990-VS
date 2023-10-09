<h1>Partition功能帮助文档</h1>

<p>
	Partition采用了一套看起来非常不寻常的方法来指定输出的词条和顺序。
	这种奇怪的方法并非我独创，甚至来说它在C/C++语言里头很常用，名字叫“位段”。
	实现方式大概就是用一个整型变量的多个特定位作为标志位，然后检测这些特定位来决定后面的操作。
	在逻辑设备中此方法也被称为“旗标”，大概意思就是这些0和1就如同信号旗，告诉程序怎么做。
	关于具体信息见<a href=https://en.wikipedia.org/wiki/Bit_field>Wikipedia</a>。
</p>

<h3>控制位段</h3>

<p>
	Partition功能执行以后将输出3个文件，分别为stars.csv，planets.csv和moons.csv，
	内容分别是恒星，行星和卫星的数据。这三个文件的输出内容由三个整型变量控制，
	可以用“-starflags=&ltflags&gt”，“-planflags=&ltflags&gt”和“-sateflags=&ltflags&gt”修改它们的值。
	其中&ltflags&gt对应一个二进制数，starflags和sateflags不能大于8位，planflags不能大于16位。
	比如输入-starflags=11111就是把输出stars.csv的控制变量修改为0b00011111。
</p>

<p>
	至于这些控制位都代表了什么，表格如下：
</p>
<table>
	<tr><th>编码</th><th>对应词条</th><th>序号</th></tr>
	<tr><th colspan=3>恒星 (8位)</th></tr>
	<tr><td>0b00000001</td><td>所在系统(质心)</td><td>0</td></tr>
	<tr><td>0b00000010</td><td>名称</td><td>1</td></tr>
	<tr><td>0b00000100</td><td>光谱型</td><td>2</td></tr>
	<tr><td>0b00001000</td><td>质量</td><td>3</td></tr>
	<tr><td>0b00010000</td><td>绝对星等</td><td>4</td></tr>
	<tr><td>0b00100000</td><td>光度</td><td>5</td></tr>
	<tr><td>0b01000000</td><td>温度</td><td>6</td></tr>
	<tr><td>0b10000000</td><td>半径</td><td>7</td></tr>
	<tr><th colspan=3>行星 (16位)</th></tr>
	<tr><td>0b0000000000000001</td><td>名称</td><td>0</td></tr>
	<tr><td>0b0000000000000010</td><td>质量</td><td>1</td></tr>
	<tr><td>0b0000000000000100</td><td>半径</td><td>2</td></tr>
	<tr><td>0b0000000000001000</td><td>表面重力</td><td>3</td></tr>
	<tr><td>0b0000000000010000</td><td>平均密度</td><td>4</td></tr>
	<tr><td>0b0000000000100000</td><td>恒星年</td><td>5</td></tr>
	<tr><td>0b0000000001000000</td><td>半长轴</td><td>6</td></tr>
	<tr><td>0b0000000010000000</td><td>离心率</td><td>7</td></tr>
	<tr><td>0b0000000100000000</td><td>倾角</td><td>8</td></tr>
	<tr><td>0b0000001000000000</td><td>温度</td><td>9</td></tr>
	<tr><td>0b0000010000000000</td><td>发现方式</td><td>A</td></tr>
	<tr><td>0b0000100000000000</td><td>发现年份</td><td>B</td></tr>
	<tr><td>0b0001000000000000</td><td>所在恒星名称</td><td>C</td></tr>
	<tr><td>0b0010000000000000</td><td>所在恒星光谱</td><td>D</td></tr>
	<tr><td>0b0100000000000000</td><td>所在恒星质量</td><td>E</td></tr>
	<tr><td>0b1000000000000000</td><td>所在恒星温度</td><td>F</td></tr>
	<tr><th colspan=3>卫星 (8位，前2位不使用)</th></tr>
	<tr><td>0b00000001</td><td>名称</td><td>0</td></tr>
	<tr><td>0b00000010</td><td>所在行星</td><td>1</td></tr>
	<tr><td>0b00000100</td><td>半径</td><td>2</td></tr>
	<tr><td>0b00001000</td><td>半长轴</td><td>3</td></tr>
	<tr><td>0b00010000</td><td>恒星月</td><td>4</td></tr>
	<tr><td>0b00100000</td><td>发现年份</td><td>5</td></tr>
</table>

<p>
	同时输出多个词条时，把每个词条对应的编码作或运算即可，简单来说就是直接把对应的标志位置1，
	不输出的词条对应标志位置0。
</p>

<h3>输出顺序</h3>

<p>
	输出的词条确定以后，词条就会按照上述表格给出的序号由小到大顺序输出，对应CSV文件从左到右。
	这个输出的顺序也是可以修改的，它由另一个整型变量控制，恒星和卫星为32位，行星为64位。
	同样的，可以使用“-starseq=&ltseqnum&gt”, “-planseq=&ltseqnum&gt”和“-sateseq=&ltseqnum&gt”修改它们的值。
	&ltseqnum&gt对应一个十六进制数，其各位的大小决定输出的顺序，采用<a href=https://en.wikipedia.org/wiki/Endianness>低字节序</a>，也就是实际顺序与输入的顺序相反。
	如-starseq=43210对应的输出顺序就是01234。
</p>

<p>
	这听起来似乎很复杂，对吗？这确实有些复杂，但是可以说简单些。
	前面提到各位数字的大小决定输出的顺序，那如果把其中任意两位对调，那输出的词条位置也就相应的对调了。
	这其实就是最基础的操作，把它重复多次即可自定义输出的顺序了。
</p>

<p>
	当然，你要先知道你之后输出的文件里头有几个词条，如果顺序的位数和词条个数不匹配，系统会自动忽略多余的位或补全缺少的位，
	缺少的位还是会按照默认顺序输出。
</p>

<p>示例：</p>

```cmd
InfoGen <Filename> -partition -starflags=11111 -starseq=43210
```

<h3>预设方案</h3>

<p>
	当然，这边也是考虑过纯小白的感受的。毕竟上面有一些专业名词不是所有人能懂的。
	所以这里提供了一些预设方案，将starflags，planflags和sateflags这些参数的值设为pre&ltx&gt可应用这些预设，设为full可以输出全部词条：
</p>

<table>
	<tr><th>预设名</th><th>编码</th><th>顺序</th></tr>
	<tr><th colspan=3>恒星</th></tr>
	<tr><td>pre1</td><td>0b00011111</td><td>0x00043210</td></tr>
	<tr><td>pre2</td><td>0b01110011</td><td>0x00042310</td></tr>
	<tr><td>pre3</td><td>0b01011011</td><td>0x00043210</td></tr>
	<tr><td>pre4</td><td>0b10000111</td><td>0x00002310</td></tr>
	<tr><td>pre5</td><td>0b01101111</td><td>0x00243510</td></tr>
	<tr><td>full</td><td>0b11111111</td><td>0x76543210</td></tr>
	<tr><th colspan=3>行星</th></tr>
	<tr><td>pre1</td><td>0b1100011001100111</td><td>0x0000000876543210</td></tr>
	<tr><td>pre2</td><td>0b0101011111100111</td><td>0x0000010A98756432</td></tr>
	<tr><td>pre3</td><td>0b0000011011110111</td><td>0x0000000817654320</td></tr>
	<tr><td>pre4</td><td>0b0000100111100011</td><td>0x0000000006543210</td></tr>
	<tr><td>pre5</td><td>0b0000101111110111</td><td>0x0000009487653210</td></tr>
	<tr><td>pre6</td><td>0b0001100011100111</td><td>0x0000000007654321</td></tr>
	<tr><td>pre7</td><td>0b0000001011001111</td><td>0x0000000004653210</td></tr>
	<tr><td>full</td><td>0b1111111111111111</td><td>0xFEDCBA9876543210</td></tr>
	<tr><th colspan=3>卫星</th></tr>
	<tr><td>full</td><td>0b00111111</td><td>0x00543210</td></tr>
<table>

<p>示例：</p>

```cmd
InfoGen <Filename> -partition -starflags=pre1
```