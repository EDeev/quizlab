from django.db import models

# Create your models here.

class Quiz(models.Model):
    title = models.CharField(max_length=200)
    description = models.TextField()
    author = models.CharField(max_length=100)
    created_at = models.DateTimeField(auto_now_add=True)
    time_limit = models.IntegerField(help_text='Ограничение по времени в минутах')
    is_published = models.BooleanField(default=False)

    def __str__(self):
        return self.title

