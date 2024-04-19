> [!NOTE]
> **Họ và tên**: Nguyễn Tường Hùng<br/>
> **MSSV**: 23020078<br/>
> **LHP**: INT2215 51<br/>
> **Tên dự án**: *Goal Dash* (về đích nhanh nhất) 🏃<br/>
> **Ngày**: 19/04/2024

# Mục Lục 📃

- [Mục Lục](#mục-lục)
- [Chủ đề](#chủ-đề)
- [Tự đánh giá](#tự-đánh-giá)
  - [Phát triển](#phát-triển)
  - [Các chức năng](#các-chức-năng)
    - [Màn hình chính](#màn-hình-chính)
    - [Play](#play)
    - [Map Building](#map-building)
    - [Settings](#settings)
  - [Một số điểm nổi bật khác do em tự phát triển](#một-số-điểm-nổi-bật-khác-do-em-tự-phát-triển)
    - [Sử dụng lớp `Vector2` để quản lý và xử lý các toạ độ 2 chiều](#sử-dụng-lớp-vector2-để-quản-lý-và-xử-lý-các-toạ-độ-2-chiều)
    - [LinkedFunction (Hàm liên kết) - ứng dụng của Singly Linked List](#linkedfunction-hàm-liên-kết---ứng-dụng-của-singly-linked-list)
    - [User Interface - Giao diện người dùng](#user-interface---giao-diện-người-dùng)
- [Demo (Youtube)](#demo-youtube)
- [Kết luận](#kết-luận)

# Chủ đề 

Vượt qua những bức tường, thu thập vàng và về đích trong thời gian ngắn nhất.

# Tự đánh giá

Em tự nhận thấy dự án của mình có chất lượng thuộc vào top **10%** của lớp, và xứng đáng được ngưỡng điểm **10**, bởi các lý do sẽ được nêu sau đây.

## Phát triển 🧑‍💻

Bắt đầu vào ngày *20/02/2024*, dự án được em phát triển dựa trên những kinh nghiệm làm game (Unity) mà em đã có trước đó.

* Trong đó, hơn *97%* code là do em tự học hỏi và phát triển, qua các nguồn:
  * [C++/SDL2 RPG Physics Based 2D Platformer for Beginners Tutorial](https://www.youtube.com/playlist?list=PL2RPjWnJduNmXHRYwdtublIPdlqocBoLS) (Toàn bộ).
  * [How To Make A Game In C++ & SDL2 From Scratch!](https://www.youtube.com/playlist?list=PLhfAbcv9cehhkG7ZQK0nfIGJC_C-wSLrx) (Một vài video đầu).
  * Bing Copilot.

* Số còn lại, em tham khảo code ở một số nơi và sử dụng các Chatbot AI như ***ChatGPT, Bing Copilot, Gemini*** để hỗ trợ viết và chỉnh sửa các phần code để tối ưu nhất.

Các phần mềm em đã sử dụng trong quá trình phát triển dự án:

* **IDE**: *Visual Studio Code*.
* **Chỉnh sửa hình ảnh**: *Adobe Photoshop, Aseprite*.
* **Chỉnh sửa âm thanh**: *Audacity*.
* **Một số phần mềm ghi chú để lên ý tưởng, phát triển thuật toán**: *Notion, Microsoft Whiteboard, Office OneNote*.

## Các chức năng

### Màn hình chính

<img src="images\image-20240419142012864.png" alt="image-20240419142012864" style="zoom:50%;" />

Màn hình chính gồm tiêu đề, và các nút điều hướng sau:

* **Start**: Chọn map và bắt đầu chơi.
* **Map Building**: Chế độ tạo map.
* **Settings**: Cài đặt.
* **About**: Mở trang GitHub [hxngxd (Nguyen Tuong Hung)](https://github.com/hxngxd) trên trình duyệt.
* **Exit**: Thoát game.

Để ý rằng, các layer tách biệt của background di chuyệt mượt mà, ngược chiều so với con trỏ chuột (layer gần hơn thì di chuyển nhanh hơn), các chấm sáng thay đổi độ sáng.

Con trỏ chuột được thay bằng hình ảnh khác.

### Play

<img src="images\image-20240419143130055.png" alt="image-20240419143130055" style="zoom:50%;" />

Tại đây người chơi được chọn các map theo thứ tự để chơi.

* **All**: Chọn hoặc bỏ chọn tất cả các map.

* **Play now**: Bắt đầu chơi (chỉ ấn được khi có ít nhất 1 map được chọn).

<img src="images\image-20240419143321591.png" alt="image-20240419143321591" style="zoom:50%;" />

Trong khi chơi, các thông số được hiện là **điểm, thời gian chơi, map hiện tại, máu còn lại của người chơi**. Còn về phần độ khó của map, em vẫn chưa tìm ra cách để đánh giá tự động độ khó của map.

Bây giờ background thay vì di chuyển so với chuột thì sẽ di chuyển so với người chơi.

Về xử lý va chạm, em sử dụng thuật toán va chạm ***axis-aligned bounding boxes (AABB)*** kết hợp với thuật toán ***tìm kiếm theo chiều rộng (BFS)*** để xử lý.

Phía bên phải có các nút điều hướng để thoát game, về màn hình chính, cài đặt.

<img src="images\image-20240419143637939.png" alt="image-20240419143637939" style="zoom:50%;" />

Khi thu thập được đồng vàng cuối cùng, cánh cổng đến map tiếp theo sẽ mở ra. Người chơi đi vào để chuyển sang map đó.

***Giữa hai map có transition rất mượt, các ô sẽ nhỏ dần và lớn dần theo thứ tự từ trên xuống.***

|                Màn hình khi người chơi thắng                 |                 Màn hình khi người chơi thua                 |
| :----------------------------------------------------------: | :----------------------------------------------------------: |
| <img src="images\image-20240419144308955.png" alt="image-20240419144308955" style="zoom: 33%;" /> | <img src="images\Screenshot 2024-04-19 144206.png" alt="Screenshot 2024-04-19 144206" style="zoom: 33%;" /> |

### Map Building

<img src="images\image-20240419145109354.png" alt="image-20240419145109354" style="zoom:50%;" />

Trong chế độ này, người dùng có thể:

* Tạo map tự động, "ngẫu nhiên" (có thể thay đổi các khả năng xuất hiện của các loại ô trong phần **Settings**).

* Hoặc sử dụng các nút ở hàng dưới màn hình để vẽ và chỉnh sửa các map với chuột.

  * Có các loại vẽ được đó là: `WALL`, `COIN`, `HEALTH`, `SPAWN` (duy nhất), `WIN` (duy nhất). 

* Mở map đã tồn tại:

  <img src="images\image-20240419145512768.png" alt="image-20240419145512768" style="zoom: 50%;" />

* Lưu map: Lưu vào map hiện tại (nếu đang mở một map đã tồn tại), hoặc lưu map với tên mới (độ dài tối đa 15 ký tự).

  | <img src="images\image-20240419145622072.png" alt="image-20240419145622072" style="zoom: 33%;" /> | <img src="images\image-20240419145646098.png" alt="image-20240419145646098" style="zoom: 33%;" /> |
  | :----------------------------------------------------------: | :----------------------------------------------------------: |

*Về phần tạo map tự động (Random), do game của em có chứa yếu tố trọng lực, và cần phải random được map có thể chơi được và đủ thách thức nên việc này không dễ dàng. Em đã kết hợp các thuật toán sau đây:*

* **Perlin Noise (tạo nhiễu ngẫu nhiên)**.
  * Em sử dụng mã nguồn mở tại: [Reputeless/PerlinNoise: Header-only Perlin noise library for modern C++](https://github.com/Reputeless/PerlinNoise).
* **Tìm kiếm theo chiều sâu (DFS)** để kiểm tra sự liên thông của các ô trống (`EmptyToEmpty`).
* Một số thuật toán tìm kiếm ô người chơi có thể đến **do tự em phát triển** (`Trajectory`, `Horizontal`, `DownVertical`).
  * Vì thuật toán trong hàm `Trajectory` liên quan đến các thuộc tính về tốc độ của người chơi, nên việc thay đổi các tốc độ trong settings cũng có ảnh hưởng đến đầu ra của map.


### Settings

<img src="images\image-20240419150911367.png" alt="image-20240419150911367" style="zoom: 50%;" />

**Người dùng có thể tinh chỉnh được nhiều thứ như:**

* Độ phân giải (yêu cầu khởi động lại do các tính toán về toạ độ và kích cỡ được thực hiện khi mở game).
* Tốc độ di chuyển, nhảy, gia tốc, trọng lực của người chơi và các thuộc tính khác.
  * Người dùng có thể bay khi trọng lực bằng 0.

* Tắt bật background, âm thanh, âm lượng, padding box, thời gian,...
* Thay đổi tốc độ transition của map.
* Thay đổi layout phím chơi: (`W`, `A`, `S`, `D`, `SPACE`) hoặc (`UP`, `LEFT`, `DOWN`, `RIGHT`, `ENTER`).
  * Khi trọng lực bằng 0, dùng phím `W` hay `UP` để bay lên, `S` hay `DOWN` để bay xuống. Ngược lại, dùng `SPACE` hoặc `ENTER` để nhảy lên.
* Chỉnh sửa **%** các khả năng xuất hiện ở trong chế độ tạo map.
* Lưu settings, reset về settings mặc định (yêu cầu khởi động lại do các lý do về đồng bộ giá trị).
  * Khi bắt đầu khởi chạy, chương trình sẽ kiểm tra file config ở ngoài có hợp lệ hay không, rồi sau đó nhập các giá trị vào cho Settings.

***Đặc biệt người dùng có thể chọn nhạc và thêm nhạc theo ý thích:***

<img src="images\image-20240419151613392.png" alt="image-20240419151613392" style="zoom:50%;" />

Người dùng chỉ cần kéo thả file nhạc ***.mp3*** vào thư mục ***sound/musics/***, game sẽ tự động đọc và cập nhật trong settings.

<img src="images\Screenshot 2024-04-19 151738.png" alt="Screenshot 2024-04-19 151738" style="zoom:50%;" />

## Một số điểm nổi bật khác do em tự phát triển

### Sử dụng lớp `Vector2` để quản lý và xử lý toạ độ 2 chiều

```cpp
struct Vector2
{
    float x, y;
    ...
};
```

Có sử dụng nạp chồng toán tử và nhiều hàm khác để xử lý.

### LinkedFunction (Hàm liên kết) - ứng dụng của Singly Linked List

```cpp
class FunctionNode
{
  public:

    ...
};

class LinkedFunction
{
  public:

    ...
};
```

Vì tất cả mọi thứ trong game đều chạy trong một hàm `while`, nên hàm liên kết có những chức năng rất hữu ích sau đây:

* Chạy một hàm cho đến khi một điều kiện nào đó hết thoả mãn.

* Chạy một hàm với số lần cho trước.

* Delay lời gọi hàm với khoảng thời gian cho trước. Ví dụ với đoạn code sau:

  ```cpp
  LinkedFunction *lf = new LinkedFunction(
      []() {
          print("Hello world");
          return 1;
      },
      1000);
  lf->Execute();
  ```

  * Dòng lệnh `print("Hello world");` sẽ được thực thi sau `1000ms` tính từ lúc gọi `Execute()`.

**Transition** mượt mà giữa các map chính là một trong những ứng dụng của hàm liên kết.

### User Interface - Giao diện người dùng

Các thành phần của UI đều được do em tự phát triển bao gồm:

* **Canvas**.
* **Button**.
* **Text**.
* **Toggle Switch**.
* **Slider**.
* **Input field** (về cơ bản vẫn chỉ là **Text**, thêm caret nhấp nháy, thay đổi chữ khi gõ, chứ chưa phải là Input field hoàn toàn, và mới chỉ được áp dụng khi gõ tên lưu map).

**Canvas** như là một khung, chỉ cần đặt các thành phần khác vào, canvas sẽ tự động căn chỉnh vị trí và kích cỡ các thành phần đó dựa trên những tham số như: ***spacing, margin, alignment,...***

Về phần render chữ, em sử dụng thuật toán **tìm kiếm nhị phân (binary search)** để tính toán cỡ chữ vừa đủ với kích cỡ nền cho trước. Và có thể chọn căn trái, căn giữa, căn phải cho chữ.

# Demo (Youtube)

[![](https://img.youtube.com/vi/JG7NuMQjLXo/maxresdefault.jpg)](https://www.youtube.com/watch?v=JG7NuMQjLXo)

> Ấn vào ảnh để chuyển hướng đến video demo.

# Kết luận

Với những lý do trên, em nghĩ mình xứng đáng lọt vào top **10%** của lớp.

Và cũng qua quá trình phát triển dự án này, em đã học được nhiều điều như sau:

* Hiểu rõ hơn về lập trình hướng đối tượng, tính kế thừa và mối quan hệ giữa các đối tượng với nhau.
* Cách quản lý các thư mục, tập tin (headers, cpp,...). Cách đọc, ghi, xử lý các tệp ngoài chương trình.
* Cách tổ chức một chương trình tối ưu và gọn gàng hơn.
* Hiểu rõ hơn về cách sử dụng con trỏ, danh sách liên kết, cấp phát và thu hồi bộ nhớ.
* Hiểu rõ hơn về cách giao diện người dùng hoạt động.
* Hiểu rõ hơn về chữ ký hàm, hàm nạp chồng, toán tử nạp chồng.

**Em xin chân thành cảm ơn!**