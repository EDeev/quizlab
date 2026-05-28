from django.contrib import admin
from django.urls import path, include
from rest_framework import routers
from quiz.views import QuizViewSet

router = routers.DefaultRouter()
router.register(r'quiz', QuizViewSet)

urlpatterns = [
    path('admin/', admin.site.urls),
    path('api/', include(router.urls)),
]

