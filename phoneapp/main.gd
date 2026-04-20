extends Control

const REFRESH_URL:String = "http://127.0.0.1:6768"
const DOWNLOAD_URL:String = "https://cool.foggydude.dev/?url={0}"

@onready var download:AwaitableHTTPRequest = $Downloader
@onready var player:AudioStreamPlayer = $Player
@onready var info_label:Label = $InfoLabel
@onready var refresh_timer: Timer = $RefreshTimer

#TODO LIST:
#Do a request every like 2secs to the Arduino to see what song to play
#Get name of the song played (maybe get it from the arduino)
#Cool awesome interface

func _ready():
	pass
	#info_label.text = "Loading..."
	#await download_audio("ranweBZaoTQ")
	#info_label.text = "Playing! i dont know the name though"

func get_audio_data():
	print("trying to get data")
	var resp := await download.async_request(REFRESH_URL)
	if !resp.success() or resp.status_err():
		push_error("Request failed.")
	
	print("Success")
	var output = resp.body_as_string()
	print(resp.bytes)
	refresh_timer.start()

func download_audio(video_url:String):
	var resp := await download.async_request(DOWNLOAD_URL.format([video_url]))
	if !resp.success() or resp.status_err():
		push_error("Request failed.")

	load_audio(resp.bytes)

func load_audio(bytes:PackedByteArray):
	var audio := AudioStreamMP3.new()
	audio.data = bytes
	player.stream = audio
	player.play()
