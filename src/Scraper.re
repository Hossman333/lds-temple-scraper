module Request = {
  type error;
  type response = {statusCode: int};
  type htmlBody;
  type requestOptions('a) = {
    .
    "uri": string,
    "encoding": 'a,
  };
  [@bs.module]
  external request :
    (
      requestOptions('a),
      (error, response, Js.Nullable.t(htmlBody)) => unit
    ) =>
    unit =
    "";
};

module NodeMore = {
  type writeStream;
  [@bs.module "fs"] external createWriteStream : string => writeStream = "";
  [@bs.send.pipe: writeStream]
  external write : (Request.htmlBody, unit => unit) => unit = "";
  [@bs.module "fs"]
  external close : (Request.htmlBody, unit => unit) => unit = "";
};

module Cheerio = {
  type loadedCheerio;
  [@bs.module]
  external cheerio : (string, Request.htmlBody) => loadedCheerio = "";
  [@bs.send.pipe: loadedCheerio] external text : unit => string = "";
  [@bs.send.pipe: loadedCheerio] external attr : string => string = "";
  [@bs.send.pipe: loadedCheerio]
  external children : unit => loadedCheerio = "";
  [@bs.send.pipe: loadedCheerio] external first : unit => loadedCheerio = "";
  [@bs.send.pipe: loadedCheerio]
  external each : ((int, string) => unit) => unit = "";
};

let url = "http://en.wikipedia.org/wiki/List_of_temples_of_The_Church_of_Jesus_Christ_of_Latter-day_Saints";

type templeData = {
  .
  "location": string,
  "imageUrl": string,
};

let temples: array(templeData) = [||];

let templeImages: array(string) = [||];

let skipTemples: list(string) = [
  "Papeete Tahiti Temple",
  "Seoul Korea Temple",
  "Cebu City Philippines Temple",
  "Kyiv Ukraine Temple",
  "Trujillo Peru Temple",
  "Cedar City Utah Temple",
];

let downloadAllImages = (templeArray: array(templeData)) => {
  Js.log("Beginning to download temple images.....");
  templeArray
  |> Js.Array.forEach((templeInfo: templeData) => {
       let templeUrl = "https:" ++ templeInfo##imageUrl;
       Request.request(
         {"uri": templeUrl, "encoding": Js.Null.empty}, (_, _, body) =>
         switch (Js.Nullable.toOption(body)) {
         | None => ()
         | Some(body) =>
           Js.log("Downloading " ++ templeInfo##location);
           NodeMore.createWriteStream(
             "./temples/" ++ templeInfo##location ++ ".jpg",
           )
           |> NodeMore.write(body, () => Js.log("Done."));
         }
       );
     });
};

Request.request({"uri": url, "encoding": "utf8"}, (_, _, maybeBody) =>
  switch (Js.Nullable.toOption(maybeBody)) {
  | None => ()
  | Some(htmlBody) =>
    Js.log2("URL:", url);
    let templeNameScrape = Cheerio.cheerio(".fn", htmlBody);
    templeNameScrape
    |> Cheerio.each((_, elem) => {
         let temple = Cheerio.cheerio(elem, htmlBody);
         let templeLoc =
           temple |> Cheerio.children() |> Cheerio.first() |> Cheerio.text();
         if (String.length(templeLoc) !== 0) {
           let data: templeData = {"location": templeLoc, "imageUrl": ""};
           Js.Array.push(data, temples) |> ignore;
           ();
         };
       });
    let templeImageScrape = Cheerio.cheerio(".image", htmlBody);
    templeImageScrape
    |> Cheerio.each((_, elem) => {
         let temple = Cheerio.cheerio(elem, htmlBody);
         let templeSrc =
           temple
           |> Cheerio.children()
           |> Cheerio.first()
           |> Cheerio.attr("src")
           |> Js.String.make;
         let templeNaturalWidth =
           temple
           |> Cheerio.children()
           |> Cheerio.first()
           |> Cheerio.attr("data-file-width")
           |> Js.String.make;
         let widthToReplace =
           templeSrc
           |> Js.String.substring(
                ~from=Js.String.lastIndexOf("/", templeSrc) + 1,
                ~to_=Js.String.lastIndexOf("px", templeSrc),
              );
         let newUrl =
           templeSrc |> Js.String.replace(widthToReplace, templeNaturalWidth);
         Js.Array.push(newUrl, templeImages) |> ignore;
         ();
       });
    let updatedTempleImages = Js_array.sliceFrom(4, templeImages);
    let trimmedTempleImages =
      updatedTempleImages
      |> Js_array.copy
      |> Js_array.spliceInPlace(~pos=0, ~remove=4, ~add=[||]);
    let restOfTempleImages =
      updatedTempleImages
      |> Js_array.copy
      |> Js_array.spliceInPlace(
           ~pos=5,
           ~remove=Js_array.length(updatedTempleImages),
           ~add=[||],
         );
    let finalTempleImages =
      Js_array.concat(restOfTempleImages, trimmedTempleImages);
    let skipCounter = ref(0);
    let updatedTemples =
      temples
      |> Js_array.reverseInPlace
      |> Js_array.sliceFrom(22)
      |> Js_array.reverseInPlace;
    updatedTemples
    |> Js_array.forEachi((temple, i) => {
         let templeImgOrString =
           if (Js_array.length(finalTempleImages) > i) {
             switch (
               List.find(
                 skipTemp => skipTemp === updatedTemples[i]##location,
                 skipTemples,
               )
             ) {
             | exception Not_found => finalTempleImages[i - skipCounter^]
             | _skip =>
               skipCounter := skipCounter^ + 1;
               "";
             };
           } else {
             "";
           };
         updatedTemples[i] = {
           "location": temple##location,
           "imageUrl": templeImgOrString,
         };
       });
    let encode = data =>
      Json.Encode.(
        object_([
          ("location", string(data##location)),
          ("imageUrl", string(data##imageUrl)),
        ])
      );
    let encodeAll = all => Json.Encode.array(encode, all);
    let json = encodeAll(updatedTemples);
    Node.Fs.writeFileAsUtf8Sync("temples.json", Json.stringify(json));
    downloadAllImages(updatedTemples);
  }
);
