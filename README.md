Набор потокобезопасных контейнеров для параллельной обработки большого количества объектов.

ClusterMap — аналог std::map, распределенный на множество кластеров-карт с блокированием только одного из них при записи.

ClusterGroup — набор кластеров-оберток над std::set с двухэтапным внесением изменений (буферизация, применение). Содержит в себе функционал для синхронизации потоков (по одному на каждый кластер) по фазам.