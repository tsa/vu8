var io = loadmodule('io'),
    console = loadmodule('console')

var writer = new io.FileWriter()

if (! writer.open("punko")) {
    console.println("could not load file for read")
}
else {
    writer.println("hello friend")
    writer.println("fond of the night")
    writer.close()
}

var reader = new io.FileReader()
if (! reader.open("punko"))
    console.println("could not load file for write")
else for (var line = reader.getln(); line; line = reader.getln()) {
    console.println("tata",line)
}

