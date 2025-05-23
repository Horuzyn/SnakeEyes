# SnakeEyes
- Họ và tên sinh viên: Lương Hiển Long
- MSV: 24022810
- Đề tài trò chơi: rắn săn mồi cổ điển
- Đề xuất ngưỡng điểm: 8.0

## Mô tả trò chơi
- SnakeEyes là một trò chơi dựa trên mô típ game rắn săn mồi cổ điển. Cố gắng để đạt được điểm cao nhất có thể bằng cách ăn mồi và đồng thời, tránh va chạm để có thể tiếp tục được trò chơi.

## Cách chơi

### Bắt đầu trò chơi:
- Khi khởi động gane, người chơi sẽ được đưa tới menu chính với 4 nút: Play, Score, Wall Mode, Background Sound
- Ấn nút Play để bắt đầu trò chơi
- Nếu người chơi muốn tắt nhạc nền, có thể ấn vào nút Background Sound ở góc phải phía trên
- Bên cạnh nút Background Sound là Wall Mode, khi bắt đầu trò chơi, sẽ mặc định là không xuyên tường. Người chơi có thể ấn để chuyển sang chế độ xuyên tường
### Trong khi chơi:
- Người chơi có thể sử dụng các phím mũi tên hoặc tổ hợp WASD để di chuyển con rắn cố gắng ăn các điểm hiện trên bản đồ
- Trong khi chơi, nếu muốn pause game, người chơi có thể ấn phím P
- Trong khi chơi, người chơi sẽ cố gắng không va chạm.
  - Nếu người chơi chơi ở chế độ Wall Off thì sẽ tránh va chạm tường và bản thân con rắn
  - Nếu người chơi  chơi ở chế độ WAll On thì chỉ cần tránh va chạm với bản thân con răn
### Kết thúc trò chơi
- Nếu bị va chạm, trò chơi sẽ kết thúc, khi đó người chơi sẽ thấy tổng điểm mình đã đạt được ở lần chơi đó
- Người chơi có thể chơi lại khi ấn "Again" hoặc ấn thoát với nút bên cạnh

## Cánh tính điểm
- Táo đỏ: +10 điểm
- Táo vàng: +30 điểm, -2 speed (speed càng thấp snake càng di chuyển nhanh)

## Về source code
- snake.h: Quản lý con rắn (Player), gồm các thuộc tính như tọa độ, chiều dài thân rắn, và các phương thức như update(), render(), và hasCollided() để cập nhật và vẽ rắn.
- sound.h: Xử lý âm thanh, sử dụng thư viện SDL_mixer để phát nhạc nền và hiệu ứng âm thanh như khi rắn ăn mồi.
- defs.h: Chứa các hằng số như kích thước màn hình (SCREEN_WIDTH, SCREEN_HEIGHT), tiêu đề cửa sổ.
- fruit.h: Quản lý mồi và super fruit, xác định cách thức spawn và render chúng.
- graphics.h: Chịu trách nhiệm vẽ và hiển thị các thành phần của game, như nền, nút bấm, điểm số, và các texture.
- main.cpp: Làm nhiệm chính để khởi chạy game, bao gồm vòng lặp game, xử lý sự kiện, điểm số, và quản lý các module.
