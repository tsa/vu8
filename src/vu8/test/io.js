var io = loadmodule('io'),
    console = loadmodule('console')

var writer = new io.FileWriter("punko")

if (! writer.is_open()) {
    console.println("could not load file for write")
}
else {
    writer.println("hello friend")
    writer.println("fond of the night")
    writer.close()
}

var reader = new io.FileReader("punko")
if (! reader.is_open()) {
    console.println("could not load file for read")
}
else {
    for (var line = reader.getln(); line; line = reader.getln()) {
        console.println("tata",line)
    }
}

