Реализуется экран из светодиодов 7x7, “движение” по экрану осуществляется с помощью джойстика KY-023, имеющим два аналоговых выхода потенциометра и один цифровой - датчик зажатия кнопки. Этот выход обрабатывается микроконтроллером Arduino Nano с помощью прерываний. Само нажатие кнопки оставляет выбранный светодиод включенным/выключенным, таким образом, на экране можно “рисовать”.
Для использования достаточно в коде изменить X_PINS[] и Y_PINS[], содержащие номера выходов платы.
