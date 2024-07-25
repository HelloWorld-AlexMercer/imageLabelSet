### LabelSet

The project aims to facilitate the manual addition of category labels (1, 2, 3, ...) to all images within a single folder (dataset) when creating datasets for classification tasks, and to export a JSON file where the key is the filename of the images in the folder and the value is the category label.


input dir:
```
/home/xxx/dataset/1.png
/home/xxx/dataset/2.png
/home/xxx/dataset/3.png
```

output json:
```
{
	"1.png": 2,
	"2.png": 1,
	"3.png": 2;
}
```

Tips: 
- Can import JSON files that have already been partially marked.
- This project is written in Qt C++, and the compiler version is Qt6.7.2. The source code is located in src/labelset. The project only provides executable programs for the Windows system. If you wish to run it on Linux, please compile it yourself.