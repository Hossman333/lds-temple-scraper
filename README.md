# LDS Wikipedia Temple Scraper

[Wikipedia Page](http://en.wikipedia.org/wiki/List_of_temples_of_The_Church_of_Jesus_Christ_of_Latter-day_Saints)

Simple web scraper written in ReasonML that returns json of temples. Includes the temple name and full res image path.
```
[
  {
		"location": "Taipei Taiwan Temple",
		"imageUrl": "//upload.wikimedia.org/wikipedia/commons/thumb/2/2d/Taipei_Taiwan_Temple-cropped.JPG/2210px-Taipei_Taiwan_Temple-cropped.JPG"
	},
	{
		"location": "Guatemala City Guatemala Temple",
		"imageUrl": "//upload.wikimedia.org/wikipedia/commons/thumb/7/7b/Guatemala_City_Temple_by_rkuhnau.jpg/3648px-Guatemala_City_Temple_by_rkuhnau.jpg"
	},
]
```

Also, naively downloads all images into a folder called `temples`.

## Warning

Naturally because it's dependent on wikipedia's page this will break.

---

Enjoy! 😊

[@hossman333](https://twitter.com/hossman333)
