import Qt 4.7
import Qt.location 5.0

Rectangle {
    id: fullView
    width: 640
    height: 360

    SearchResultModel {
        id: resultModel
        searchCenter:
                Coordinate {
                    latitude: 53
                    longitude: 10
                }
        didYouMean: 5
        //onQueryFinished: console.log("datareceived " + error)
    }
    Text {
        id: queryText
        text: "Query:"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        font.pixelSize: 18
    }

    ListView {
        id: placesList
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.top: suggestionsList.bottom
        anchors.right: categoriesList.left
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: 20
        model: resultModel
        delegate: Component {
            Item {
                width: parent.width; height: 170
                Column {
                    Text { text: searchResult.type == SearchResult.Place ? '<b>Name: </b> ' + searchResult.place.name : '<b>DYM: </b> ' + searchResult.didYouMeanSuggestion; font.pixelSize: 16 }
                    Text { text: '<b>Street: </b> ' + searchResult.place.location.address.street; font.pixelSize: 16 }
                    Text { text: '<b>Latitude: </b> ' + searchResult.place.location.displayPosition.latitude; font.pixelSize: 16 }
                    Text { text: '<b>Longitude: </b> ' + searchResult.place.location.displayPosition.longitude; font.pixelSize: 16 }
                    Text { text: '<b>Cat[0] id: </b> ' + searchResult.place.categories[0].categoryId; font.pixelSize: 16 }
                    Text { text: '<b>Media count: </b> ' + searchResult.place.media.data.length; font.pixelSize: 16 }
                    //Text { text: '<b>All media count: </b> ' + searchResult.place.mediaCount; font.pixelSize: 16 }
                    Text { text: '<b>Descriptions count: </b> ' + searchResult.place.descriptions.length; font.pixelSize: 16 }
                    //Text { text: '<b>Reviews count: </b> ' + searchResult.place.reviews.data.length; font.pixelSize: 16 }
                    //Text { text: '<b>All reviews count: </b> ' + searchResult.place.reviewCount; font.pixelSize: 16 }
                    Text { text: '<b>Tags: </b> ' + searchResult.place.tags; font.pixelSize: 16 }
                    //Text { text: '<b>Suppliers: </b> ' + JSON.stringify(searchResult.place.suppliers); font.pixelSize: 16 }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log("clicked " + searchResult.place.name)
                        placeManager.getPlaceDetails(searchResult.place)
                        //placeManager.getPlaceReviews(searchResult.place, 10, 50)
                        //placeManager.getPlaceMedia(searchResult.place)
                    }
                    onPressAndHold: {
                        console.log("longpress " + searchResult.place.name);
                        placesList.model = recommendationModel;
                        recommendationModel.placeId = searchResult.place.placeId;
                        recommendationModel.executeQuery();
                    }
                }
            }
        }
    }

    PlaceManager {
        id: placeManager
    }

    Rectangle {
        id: searchTermRect
        color: "#ffffff"
        anchors.bottom: suggestionsList.top
        anchors.bottomMargin: 10
        anchors.right: searchButton.left
        anchors.rightMargin: 5
        anchors.left: queryText.right
        anchors.leftMargin: 5
        anchors.top: parent.top
        anchors.topMargin: 10
        border.color: "#23b98a"

        TextEdit {
            id: search_term
            text: "p"
            anchors.fill: parent
            clip: false
            opacity: 1
            font.pixelSize: 18
        }
    }

    Rectangle {
        id: searchButton
        width: 70
        color: "#3be7ec"
        anchors.bottom: suggestionsList.top
        anchors.bottomMargin: 10
        anchors.right: suggestionButton.left
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        border.color: "#2b1ae2"
        MouseArea {
            id: searchButtonArea
            anchors.fill: parent
            onClicked:
            {
                placesList.model = resultModel;
                resultModel.clearCategories();
                resultModel.searchTerm = search_term.text;
                resultModel.executeQuery();
            }
        }

        Text {
            id: searchText
            color: "#fbf8f8"
            text: "Search"
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "MS UI Gothic"
            font.pixelSize: 15
        }
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#000000"
            }

            GradientStop {
                position: 1
                color: "#ffffff"
            }
        }
    }

    TextPredictionModel {
        id: resultSuggestion
        searchCenter:
            Coordinate {
                latitude: 50.05
                longitude: 19.90
            }
        offset: 0
        limit: 15
        searchTerm: search_term.text
    }

    RecommendationModel {
        id: recommendationModel
        searchCenter:
                Coordinate {
                    latitude: 50.05
                    longitude: 19.90
                }
        offset: 0
        limit: 15
        //onQueryFinished: console.log("datareceived " + error)
    }

    Rectangle {
        id: suggestionButton
        width: 70
        color: "#3be7ec"
        anchors.bottom: suggestionsList.top
        anchors.bottomMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        border.color: "#2b1ae2"
        MouseArea {
            id: suggestionButtonArea
            anchors.fill: parent
            onClicked: {
                resultSuggestion.executeQuery()
            }
        }

        Text {
            id: suggestionText
            color: "#fbf8f8"
            text: "Suggest"
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "MS UI Gothic"
            font.pixelSize: 15
        }
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#000000"
            }

            GradientStop {
                position: 1
                color: "#ffffff"
            }
        }
    }

    ListView {
        id: suggestionsList
        model: resultSuggestion
        delegate: Component {
            Item {
                width: parent.width; height: 15
                Column {
                    Text { text: '<b>Suggestion:</b> ' + textPrediction; font.pixelSize: 16 }
                }
            }
        }
        height: 60
        anchors.top: queryText.bottom
        anchors.topMargin: 10
        anchors.bottomMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: categoriesList.left
        anchors.rightMargin: 10
    }

    Text {
        id: categoriesText
        x: 226
        y: 10
        width: 150
        height: 20
        text: "Categories tree:"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.top: queryText.bottom
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        font.pixelSize: 18
    }

    SupportedCategoriesModel {
        id: categoriesModel
    }

    ListView {
        id: categoriesList
        width: 125
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: categoriesText.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        model: categoriesModel
        delegate: Component {
            Item {
                height: 40
                width: parent.width
                Column {
                    Text { text: '<b>name:</b> ' + category.name; font.pixelSize: 16 }
                    Text { text: '<b>id:</b> ' + category.categoryId; font.pixelSize: 16 }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        //console.log("clicked = " + category.categoryId)
                        //console.log("clicked = " + JSON.stringify(modelData))
                        placesList.model = resultModel;
                        resultModel.clearSearchTerm();
                        resultModel.searchCategory = category;
                        resultModel.executeQuery()
                    }
                }
            }
        }
    }
}