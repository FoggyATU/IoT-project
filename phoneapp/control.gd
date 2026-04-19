extends Control

const URL:String = "https://cool.foggydude.dev/?url={0}"

@onready var http:AwaitableHTTPRequest = $Req
@onready var player:AudioStreamPlayer = $AudioStreamPlayer
@onready var info_label:Label = $InfoLabel


func _ready():
	info_label.text = "Loading..."
	await download_audio("ranweBZaoTQ")
	info_label.text = "Playing! i dont know the name though"
	
func download_audio(video_url:String):
	var resp := await http.async_request(URL.format([video_url]))
	if !resp.success() or resp.status_err():
			push_error("Request failed.")
			
	var audio := AudioStreamMP3.new()
	audio.data = resp.bytes
	player.stream = audio
	player.play()
