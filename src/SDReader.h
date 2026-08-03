/*

    Замечания относительно полей currentIndex и minIndex.
    -----
    Так как список файлов является полным листингом текущего каталога,
    то он может содержать как файлы, так и вложенные каталоги. 
    Причём вложенные каталоги в отсортированном списке идут первыми.

    Таким образом первый файл может иметь индекс больший  0. Вот как раз этот инжекс 
    и содержит поле minIndex. Получается, что при последовательном перечислении
    файлов - не каталогов - мы должны начинать с minIndex и двигаться до entries.size()-1.
    
    Поле currentIndex при создании списка инициализируется UINT16_MAX. 
    Это призак того, что мы еще не двигались по списку. После того, как 
    мы начали движение по списку вперед (т.е. вызвали nextFile) поле currentIndex 
    становится равным minIndex.

*/
#ifndef SDREADER_H
#define SDREADER_H

#include <vector>
#include <algorithm>
#include <SD.h>

/**
    @brief  Тип файла
*/
enum class AudioFileEntryType {
    Directory,  // Каталог
    MP3,        // MP3-файл
    Unknown     // Неизвестный (все остальные файлы, кроме перечисленных выше)
};

/**
    @brief Тип элемента списка файлов 
*/
struct AudioFileEntry {
    String name;              // Имя файла
    AudioFileEntryType type;  // Тип аудио файла
};

/**
    @brief Класс для чтения SD-карты

    - установка текущего каталога;
    - чтение текущего каталога;
    - фильтрация файлов по расширению;
    - сортировка файлов и каталогов;
    - перемещение по списку.
*/
class SDReader {
  public:
    /**
        @brief Устанавливает текущий каталог.

        Далее происходит создание сортированного списка каталогов и файлов.
    */
    void setCurrentDir(String dir) {
        currentDir = dir;
        read();
    } // setCurrentDir

    /** 
        @brief Возврашает полное имя текущего файла.
    */    
    String getCurrentFileName(void) {
        currentIndex = currentIndex == UINT16_MAX ? minIndex : currentIndex; 
        return currentDir + "/" + entries[currentIndex].name;
    } // getCurrentFileName

    /**
        @brief Перемещает указатель на следующий файл.
    */
    void nextFile(void) {
        if (currentIndex == UINT16_MAX) {
            currentIndex = minIndex;
            return;
        }
        if (currentIndex < entries.size() - 1) currentIndex++;
        Serial.printf("\n curr %i max %i", currentIndex, minIndex);
    } // nextFile

    /**
        @brief Перемещает указатель на предыдуший файл.
    */
    void priorFile() {
        if (currentIndex > minIndex) currentIndex--;
        Serial.printf("\n curr %i max %i", currentIndex, minIndex);
    } // priorFile

    /**
        @brief Возвращает тип элемента с индексом index. 

        Если индекс выходит за пределы списка - возвращается **AudioFileEntryType::Unknown**.
        Для получения размера списка нужно использовать **getListSize**.
    */
    AudioFileEntryType getEntryType(uint16_t index) {
        if (index > entries.size()-1) return AudioFileEntryType::Unknown;
        return entries[index].type;
    } // getEntryType

    /**
        @brief Возвращает размер списка файлов. Нумерация начинается с 0. 
    */
    uint16_t getListSize(void) {
        return entries.size();
    } // getListSize

  protected:
    String currentDir;  // Текущий каталог
    uint16_t currentIndex;
    uint16_t minIndex;
    std::vector<AudioFileEntry> entries;  // Список файлов и каталогов
    
    /**
        @brief Сортировака списка файлов.
        
        В начали идут каталоги, отсортированные по алфавиту,
        за ними удут файлы, также отсортированные по алфавиту.
    */
    void sort(std::vector<AudioFileEntry>& files) {
        std::sort(files.begin(), files.end(), [](const AudioFileEntry& a, const AudioFileEntry& b) {
            if (a.type != b.type) {
                return a.type < b.type;  // Первый признак (возрастание)
            }
            return a.name.compareTo(b.name) < 0;
        });
    }  // sort
    
    /**
        @brief Возвращает тип найденного файла.
        @param entry Указатель на файл.
        @returns Тип файла.
    */
    AudioFileEntryType getEntryType(fs::File* entry) {
        if (entry->isDirectory())
            return AudioFileEntryType::Directory;
        if (entry->isFile()) {
            String entryName = String(entry->name());
            entryName.toLowerCase();
            if (entryName.endsWith(".mp3"))
                return AudioFileEntryType::MP3;
        }
        return AudioFileEntryType::Unknown;
    }  // getEntryType
    /**
        @brief Чтение текущего каталога
    */
    void read(void) {
        entries.clear();  // очистить список файлов
        fs::File root = SD.open(currentDir); 
        fs::File entry;
        AudioFileEntryType entryType;
        while (true) {
            entry = root.openNextFile();
            if (!entry) break;  // Если ничего не нашли - заканчиваем цикл
            entryType = getEntryType(&entry);
            if (entryType == AudioFileEntryType::Unknown) continue;
            entries.push_back(AudioFileEntry{ entry.name(), entryType });
        }
        sort(entries);
        currentIndex = UINT16_MAX;
        minIndex = UINT16_MAX;
        for (uint16_t i = 0; i < entries.size()-1; i++) {
            if (getEntryType(i) == AudioFileEntryType::MP3) {
                minIndex = i;
                break;;
            }
        }
        for (uint16_t i = 0; i < entries.size(); i++)
            Serial.println(entries[i].name);
    } // read
}; // SDReader

#endif