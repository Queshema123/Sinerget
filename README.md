# Sinerget
<br/>Converter - Парсит файлы XML формата и возвращает данные в виде вектора токенов
<br/>InfoBox - Один блок для фильтра/поиска
<br/>Filter - Определяет тип данных для объекта и возвращает операции, отображение для него
<br/>FilterWidget - Модальное окно фильтра
<br/>SearchWidget - Модальное окно поиска
<br/>CustomProxyModel - Прокси-модель, выполняющая фильтрацию и поиск данных на основе Info из FilterWidget и SearchWidget
<br/>Server - Отслеживает добавленные файлы, парсит их и сохраняет в модель. Также перемещает их в указанную папку
<br/>ServerWidget - Виджет отображения данных Server'а
<br/>Info - Информация с SearchWidget/FilterWidget для CustomProxyModel
<br/>Token - Хранит информацию о метрике и ее параметрах
